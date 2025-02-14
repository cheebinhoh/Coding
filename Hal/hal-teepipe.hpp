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

template <typename T> class Hal_TeePipe : private Hal_Pipe<T> {
  using Task = std::function<void(T)>;
  using PostProcessingTask = std::function<void(std::vector<T> &)>;

  class Hal_TeePipeSource : private Hal_LimitBuffer<T>, public Hal_Io<T> {
    friend class Hal_TeePipe<T>;

  public:
    Hal_TeePipeSource(size_t capacity, Hal_TeePipe *tp)
        : Hal_LimitBuffer<T>{capacity}, m_teePipe(tp) {}

    ~Hal_TeePipeSource() = default;

    void write(T &rItem) override { write(std::move_if_noexcept(rItem)); }

    void write(T &&rItem) override {
      assert(m_teePipe);

      Hal_LimitBuffer<T>::push(rItem);

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
  };

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

  void removeHal_TeePipeSource(std::shared_ptr<Hal_TeePipeSource> &sp_tps) {
    assert(nullptr != sp_tps);
    assert(this == sp_tps->m_teePipe);

    int err = pthread_mutex_lock(&m_mutex);
    if (err) {
      throw std::runtime_error(strerror(err));
    }

    pthread_testcancel();

    auto iter =
        std::find_if(m_buffers.begin(), m_buffers.end(),
                     [sp_tps](std::shared_ptr<Hal_TeePipeSource> sp_iterTps) {
                       return sp_tps.get() == sp_iterTps.get();
                     });

    if (iter != m_buffers.end()) {
      while (sp_tps->size() > 0) {
        err = pthread_cond_wait(&m_emptyCond, &m_mutex);
        if (err) {
          throw std::runtime_error(strerror(err));
        }

        pthread_testcancel();
      }

      m_buffers.erase(iter);
      sp_tps = {};
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

  void waitForEmpty() { wait(false); }

private:
  void wait(bool noOpenSource) {
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

    Hal_Pipe<T>::waitForEmpty();
  }

  void write(T &rItem) { Hal_Pipe<T>::write(rItem); }

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

        for (auto sp_tps : m_buffers) {
          m_fillBufferCount--;

          auto data = sp_tps->read();
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

#endif /* HAL_TEEPIPE_HPP_HAVE_SEEN */
