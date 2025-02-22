/**
 * Copyright © 2024 - 2025 Chee Bin HOH. All rights reserved.
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

#ifndef HAL_TEEPIPE_HPP_HAVE_SEEN

#define HAL_TEEPIPE_HPP_HAVE_SEEN

#include "hal-io.hpp"
#include "hal-limit-buffer.hpp"
#include "hal-pipe.hpp"
#include "hal-proc.hpp"

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

namespace Hal {

template <typename T> class Hal_TeePipe : private Hal_Pipe<T> {
  using Task = std::function<void(T)>;
  using PostProcessingTask = std::function<void(std::vector<T> &)>;

  /**
   * This implements the teepipe source that client can write data into
   * the teepipe structure to send to outbound pipe.
   */
  class Hal_TeePipeSource : private Hal_LimitBuffer<T>, public Hal_Io<T> {
    friend class Hal_TeePipe<T>;

  public:
    Hal_TeePipeSource(size_t capacity, Hal_TeePipe *tp)
        : Hal_LimitBuffer<T>{capacity}, m_teePipe(tp) {}

    ~Hal_TeePipeSource() = default;

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

      Hal_LimitBuffer<T>::push(item, move);

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
    std::optional<T> read() override { return Hal_LimitBuffer<T>::pop(); }

    Hal_TeePipe *m_teePipe{};
  }; /* End of class Hal_TeePipeSource */

public:
  Hal_TeePipe(std::string_view name, Hal_TeePipe::Task fn = {},
              Hal_TeePipe::PostProcessingTask pfn = {})
      : Hal_Pipe<T>{name, fn},
        m_conveyor{std::make_unique<Hal_Proc>(std::string(name) + "-conveyor")},
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

  virtual ~Hal_TeePipe() noexcept try {
    // this is important as the conveyor thread uses the conditional variable
    // and mutex, so we need to stop the thread before destroying both objects
    // at the end of life of the Hal_TeePipe object, as otherwise, those threads
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

  Hal_TeePipe(const Hal_TeePipe<T> &halTeePipe) = delete;
  const Hal_TeePipe<T> &operator=(const Hal_TeePipe<T> &halTeePipe) = delete;
  Hal_TeePipe(const Hal_TeePipe<T> &&halTeePipe) = delete;
  Hal_TeePipe<T> &operator=(Hal_TeePipe<T> &&halTeePipe) = delete;

  /**
   * @brief The method will create a teepipe source pipe and return it to
   *        the client to be used to provide data into the teepipe structure.
   *
   * @return a shared pointer to newly created teepipe source pipe
   */
  std::shared_ptr<Hal_TeePipeSource> addHal_TeePipeSource() {
    std::shared_ptr<Hal_TeePipeSource> sp_tpSource{
        std::make_shared<Hal_TeePipeSource>(1, this)};

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
  void removeHal_TeePipeSource(std::shared_ptr<Hal_TeePipeSource> &tps) {
    assert(nullptr != tps);
    assert(this == tps->m_teePipe);

    int err = pthread_mutex_lock(&m_mutex);
    if (err) {
      throw std::runtime_error(strerror(err));
    }

    pthread_testcancel();

    auto iter =
        std::find_if(m_buffers.begin(), m_buffers.end(),
                     [tps](std::shared_ptr<Hal_TeePipeSource> sp_iterTps) {
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

    // only returns if no other object owns the Hal_TeePipeSource than
    // Hal_TeePipe
    while (noOpenSource && m_buffers.size() > 0 &&
           std::count_if(m_buffers.begin(), m_buffers.end(),
                         [](auto &item) { return item.use_count() > 1; }) > 0) {
      err = pthread_cond_wait(&m_cond, &m_mutex);
      if (err) {
        throw std::runtime_error(strerror(err));
      }

      pthread_testcancel();
    }

    // only returns if no data in buffer from Hal_TeePipeSource to conveyor
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

    return Hal_Pipe<T>::waitForEmpty();
  }

  /**
   * @brief The method moves and writes the data item through teepipe structure
   *        outbound pipe.
   *
   * @param item The data item to be moved through outbound pipe
   */
  void write(T &item) override { Hal_Pipe<T>::write(item); }

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

  std::unique_ptr<Hal_Proc> m_conveyor{};
  pthread_mutex_t m_mutex{};
  pthread_cond_t m_cond{};
  pthread_cond_t m_emptyCond{};
  size_t m_fillBufferCount{};
  std::vector<std::shared_ptr<Hal_TeePipeSource>> m_buffers{};
  Hal_TeePipe::PostProcessingTask m_postProcessingTaskFn{};
};

} // namespace Hal

#endif /* HAL_TEEPIPE_HPP_HAVE_SEEN */
