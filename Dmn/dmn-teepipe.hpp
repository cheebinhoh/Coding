/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 *
 * This class implements a tee-pipe structure where multiple clients
 * can each write data to their dedicated pipe, and data from multiple
 * input pipes are channed and converged into an output pipe.
 *
 * Data from input pipe are processed and feed to output pipe in the
 * order that it is written into input pipe. If there are two input
 * pipes, then output pipe will receive one data from each, and the
 * order of which data from which input file comes first can be
 * enforced through the PostProcessingTask functor which will see
 * one data from each input pipe in a vector prior to feeding to
 * output pipe.
 */

#ifndef DMN_TEEPIPE_HPP_HAVE_SEEN

#define DMN_TEEPIPE_HPP_HAVE_SEEN

#include "dmn-io.hpp"
#include "dmn-limit-buffer.hpp"
#include "dmn-pipe.hpp"
#include "dmn-proc.hpp"

#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <string_view>
#include <thread>
#include <type_traits>
#include <vector>

namespace Dmn {

template <typename T> class Dmn_TeePipe : private Dmn_Pipe<T> {
  using Task = std::function<void(T)>;
  using PostProcessingTask = std::function<void(std::vector<T> &)>;

  /**
   * This implements the teepipe source that client can write data into
   * the teepipe structure to send to outbound pipe.
   */
  class Dmn_TeePipeSource : private Dmn_LimitBuffer<T>, public Dmn_Io<T> {
    friend class Dmn_TeePipe<T>;

  public:
    Dmn_TeePipeSource(size_t capacity, Dmn_TeePipe *tp)
        : Dmn_LimitBuffer<T>{capacity}, m_teePipe(tp) {}

    ~Dmn_TeePipeSource() = default;

    /**
     * @brief The method will copy item to input source pipe to be passed
     *        through the teepipe structure.
     *
     * param item The data item to be copied into input source pipe
     */
    void write(T &item) override { write(item, false); }

    /**
     * @brief The method will move item to input source pipe to be passed
     *        through the teepipe structure.
     *
     * param item The data item to be moved into input source pipe
     */
    void write(T &&item) override {
      T movedItem = std::move_if_noexcept(item);

      write(movedItem, true);
    }

    /**
     * @brief The method will copy item to input source pipe to be passed
     *        through the teepipe structure, the item is moved if move is
     *       true
     *
     * param item The data item to be moved or copied into input source pipe
     */
    void write(T &item, bool move) {
      assert(m_teePipe);

      Dmn_LimitBuffer<T>::push(item, move);

      int err = pthread_mutex_lock(&(m_teePipe->m_mutex));
      if (err) {
        throw std::runtime_error(strerror(err));
      }

      pthread_testcancel();
      m_teePipe->m_fillBufferCount++;

      err = pthread_cond_signal(&(m_teePipe->m_cond));
      if (err) {
        pthread_mutex_unlock(&(m_teePipe->m_mutex));

        throw std::runtime_error(strerror(err));
      }

      err = pthread_mutex_unlock(&(m_teePipe->m_mutex));
      if (err) {
        throw std::runtime_error(strerror(err));
      }
    }

  private:
    std::optional<T> read() override { return Dmn_LimitBuffer<T>::pop(); }

    Dmn_TeePipe *m_teePipe{};
  }; /* End of class Dmn_TeePipeSource */

public:
  Dmn_TeePipe(std::string_view name, Dmn_TeePipe::Task fn = {},
              Dmn_TeePipe::PostProcessingTask pfn = {})
      : Dmn_Pipe<T>{name, fn},
        m_conveyor{std::make_unique<Dmn_Proc>(std::string(name) + "-conveyor")},
        m_postProcessingTaskFn{pfn} {
    int err{};

    err = pthread_mutex_init(&m_mutex, NULL);
    if (err) {
      throw std::runtime_error(strerror(err));
    }

    err = pthread_cond_init(&m_cond, NULL);
    if (err) {
      throw std::runtime_error(strerror(err));
    }

    err = pthread_cond_init(&m_emptyCond, NULL);
    if (err) {
      throw std::runtime_error(strerror(err));
    }

    runConveyorExec();
  }

  virtual ~Dmn_TeePipe() noexcept try {
    // this is important as the conveyor thread uses the conditional variable
    // and mutex, so we need to stop the thread before destroying both objects
    // at the end of life of the Dmn_TeePipe object, as otherwise, those threads
    // will fail to be awaken from the conditional variable state, and leak to
    // system wide leak of available thread resource.
    m_conveyor = {};

    pthread_cond_destroy(&m_cond);
    pthread_cond_destroy(&m_emptyCond);
    pthread_mutex_destroy(&m_mutex);
  } catch (...) {
    // explicit return to resolve exception as destructor must be noexcept
    return;
  }

  Dmn_TeePipe(const Dmn_TeePipe<T> &dmnTeePipe) = delete;
  const Dmn_TeePipe<T> &operator=(const Dmn_TeePipe<T> &dmnTeePipe) = delete;
  Dmn_TeePipe(const Dmn_TeePipe<T> &&dmnTeePipe) = delete;
  Dmn_TeePipe<T> &operator=(Dmn_TeePipe<T> &&dmnTeePipe) = delete;

  /**
   * @brief The method will create a teepipe source pipe and return it to
   *        the client to be used to provide data into the teepipe structure.
   *
   * @return a shared pointer to newly created teepipe source pipe
   */
  std::shared_ptr<Dmn_TeePipeSource> addDmn_TeePipeSource() {
    std::shared_ptr<Dmn_TeePipeSource> sp_tpSource{
        std::make_shared<Dmn_TeePipeSource>(1, this)};

    int err = pthread_mutex_lock(&m_mutex);
    if (err) {
      throw std::runtime_error(strerror(err));
    }

    pthread_testcancel();

    m_buffers.push_back(sp_tpSource);

    err = pthread_cond_signal(&m_cond);
    if (err) {
      pthread_mutex_unlock(&m_mutex);

      throw std::runtime_error(strerror(err));
    }

    err = pthread_mutex_unlock(&m_mutex);
    if (err) {
      throw std::runtime_error(strerror(err));
    }

    return sp_tpSource;
  }

  /**
   * @brief The method will remove and delete a teepipe source pipe, all data
   *        written to the source pipe will be flushed out to the teepipe
   *        structure outbound pipe.
   *
   * @param tps a shared pointer to the teepipe source pipe
   */
  void removeDmn_TeePipeSource(std::shared_ptr<Dmn_TeePipeSource> &tps) {
    assert(nullptr != tps);
    assert(this == tps->m_teePipe);

    int err = pthread_mutex_lock(&m_mutex);
    if (err) {
      throw std::runtime_error(strerror(err));
    }

    pthread_testcancel();

    auto iter =
        std::find_if(m_buffers.begin(), m_buffers.end(),
                     [tps](std::shared_ptr<Dmn_TeePipeSource> sp_iterTps) {
                       return tps.get() == sp_iterTps.get();
                     });

    if (iter != m_buffers.end()) {
      while (tps->size() > 0) {
        err = pthread_cond_wait(&m_emptyCond, &m_mutex);
        if (err) {
          throw std::runtime_error(strerror(err));
        }

        pthread_testcancel();
      }

      m_buffers.erase(iter);
      tps = {};
    }

    err = pthread_cond_signal(&m_cond);
    if (err) {
      pthread_mutex_unlock(&m_mutex);

      throw std::runtime_error(strerror(err));
    }

    err = pthread_mutex_unlock(&m_mutex);
    if (err) {
      throw std::runtime_error(strerror(err));
    }
  }

  void wait() { wait(true); }

  long long waitForEmpty() override { return wait(false); }

private:
  long long wait(bool noOpenSource) {
    int err = pthread_mutex_lock(&m_mutex);
    if (err) {
      throw std::runtime_error(strerror(err));
    }

    pthread_testcancel();

    // only returns if no other object owns the Dmn_TeePipeSource than
    // Dmn_TeePipe
    while (noOpenSource && m_buffers.size() > 0 &&
           std::count_if(m_buffers.begin(), m_buffers.end(),
                         [](auto &item) { return item.use_count() > 1; }) > 0) {
      err = pthread_cond_wait(&m_cond, &m_mutex);
      if (err) {
        throw std::runtime_error(strerror(err));
      }

      pthread_testcancel();
    }

    // only returns if no data in buffer from Dmn_TeePipeSource to conveyor
    while (m_fillBufferCount > 0) {
      err = pthread_cond_wait(&m_emptyCond, &m_mutex);
      if (err) {
        throw std::runtime_error(strerror(err));
      }

      pthread_testcancel();
    }

    err = pthread_mutex_unlock(&m_mutex);
    if (err) {
      throw std::runtime_error(strerror(err));
    }

    return Dmn_Pipe<T>::waitForEmpty();
  }

  /**
   * @brief The method moves and writes the data item through teepipe structure
   *        outbound pipe.
   *
   * @param item The data item to be moved through outbound pipe
   */
  void write(T &item) override { Dmn_Pipe<T>::write(item); }

  /**
   * @brief The method acts as a conveyorbelt to move data item from all
   *        input source pipes to outbound pipe.
   */
  void runConveyorExec() {
    m_conveyor->exec([this]() {
      while (true) {
        int err{};

        err = pthread_mutex_lock(&m_mutex);
        if (err) {
          throw std::runtime_error(strerror(err));
        }

        pthread_testcancel();

        while (m_buffers.empty() || m_fillBufferCount < m_buffers.size()) {
          err = pthread_cond_wait(&m_cond, &m_mutex);
          if (err) {
            throw std::runtime_error(strerror(err));
          }

          pthread_testcancel();
        }

        std::vector<T> postProcessingBuffers{};

        for (auto tps : m_buffers) {
          m_fillBufferCount--;

          auto data = tps->read();
          assert(data);

          postProcessingBuffers.push_back(std::move_if_noexcept(*data));
        }

        if (m_postProcessingTaskFn != nullptr) {
          m_postProcessingTaskFn(postProcessingBuffers);
        }

        for (auto &data : postProcessingBuffers) {
          write(data);
        }

        postProcessingBuffers.clear();

        err = pthread_cond_signal(&m_emptyCond);
        if (err) {
          pthread_mutex_unlock(&m_mutex);

          throw std::runtime_error(strerror(err));
        }

        err = pthread_mutex_unlock(&m_mutex);
        if (err) {
          throw std::runtime_error(strerror(err));
        }
      }
    });
  }

  /**
   * data members for constructor to instantiate the object.
   */
  std::unique_ptr<Dmn_Proc> m_conveyor{};
  Dmn_TeePipe::PostProcessingTask m_postProcessingTaskFn{};

  /**
   * data members for internal logic.
   */
  pthread_mutex_t m_mutex{};
  pthread_cond_t m_cond{};
  pthread_cond_t m_emptyCond{};
  size_t m_fillBufferCount{};
  std::vector<std::shared_ptr<Dmn_TeePipeSource>> m_buffers{};
}; /* End of class Dmn_TeePipe */

} /* End of namespace Dmn */

#endif /* End of macro DMN_TEEPIPE_HPP_HAVE_SEEN */
