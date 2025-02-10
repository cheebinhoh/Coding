#ifndef HAL_PIPE_HPP_HAVE_SEEN

#define HAL_PIPE_HPP_HAVE_SEEN

#include "hal-buffer.hpp"
#include "hal-proc.hpp"

#include <pthread.h>
#include <cstring>
#include <functional>

template <typename T>
class Hal_Pipe : public Hal_Buffer<T>, public Hal_Proc
{
  using Task = std::function<void(T)>;

 public:
  Hal_Pipe(std::string_view name, Hal_Pipe::Task fn = {}) : Hal_Proc{name}
  {
    int err = pthread_mutex_init(&m_mutex, NULL);
    if (err)
    {
      throw std::runtime_error(strerror(err));
    }

    err = pthread_cond_init(&m_emptyCond, NULL);
    if (err)
    {
      throw std::runtime_error(strerror(err));
    }

    exec([this, fn]() {
      while (true)
      {
        T &&item = this->pop();

        int errInLoop = pthread_mutex_lock(&m_mutex);
        if (errInLoop)
        {
          throw std::runtime_error(strerror(errInLoop));
        }

        pthread_testcancel();

        if (fn)
        {
          fn(std::move(item));
        }

        ++m_count;

        errInLoop = pthread_cond_signal(&m_emptyCond);
        if (errInLoop)
        {
          pthread_mutex_unlock(&m_mutex);

          throw std::runtime_error(strerror(errInLoop));
        }

        pthread_testcancel();

        errInLoop = pthread_mutex_unlock(&m_mutex);
        if (errInLoop)
        {
          throw std::runtime_error(strerror(errInLoop));
        }
      }
    });
  }

  virtual ~Hal_Pipe()
  {
    Hal_Proc::stopExec();
    pthread_cond_destroy(&m_emptyCond);
    pthread_mutex_destroy(&m_mutex);
  }

  Hal_Pipe(const Hal_Pipe<T> &halPipe) = delete;
  const Hal_Pipe<T> &operator=(const Hal_Pipe<T> &halPipe) = delete;
  Hal_Pipe(Hal_Pipe<T> &&halPipe) = delete;
  Hal_Pipe<T> &operator=(Hal_Pipe<T> &&halPipe) = delete;

  void waitForEmpty()
  {
    long long inboundCount{};

    inboundCount = Hal_Buffer<T>::waitForEmpty();

    int err = pthread_mutex_lock(&m_mutex);
    if (err)
    {
      throw std::runtime_error(strerror(err));
    }

    pthread_testcancel();

    while (m_count < inboundCount)
    {
      err = pthread_cond_wait(&m_emptyCond, &m_mutex);
      if (err)
      {
        throw std::runtime_error(strerror(err));
      }

      pthread_testcancel();
    }

    err = pthread_mutex_unlock(&m_mutex);
    if (err)
    {
      throw std::runtime_error(strerror(err));
    }
  }

 private:
  pthread_mutex_t m_mutex{};

  pthread_cond_t m_emptyCond{};

  long long m_count{};
};


#endif /* HAL_PIPE_HPP_HAVE_SEEN */
