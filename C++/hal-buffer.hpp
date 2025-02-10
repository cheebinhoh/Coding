#ifndef HAL_BUFFER_HPP_HAVE_SEEN

#define HAL_BUFFER_HPP_HAVE_SEEN

#include <algorithm>
#include <cassert>
#include <cstring>
#include <deque>

#include <pthread.h>

template <typename T>
class Hal_Buffer
{
 public:
  Hal_Buffer()
  {
    int err{};

    err = pthread_mutex_init(&m_mutex, NULL);
    if (err)
    {
      throw std::runtime_error(strerror(err));
    }

    err = pthread_cond_init(&m_cond, NULL);
    if (err)
    {
      throw std::runtime_error(strerror(err));
    }

    err = pthread_cond_init(&m_emptyCond, NULL);
    if (err)
    {
      throw std::runtime_error(strerror(err));
    }
  }

  virtual ~Hal_Buffer()
  {
    pthread_cond_destroy(&m_emptyCond);
    pthread_cond_destroy(&m_cond);
    pthread_mutex_destroy(&m_mutex);
  }

  Hal_Buffer(const Hal_Buffer<T> &halBuffer) = delete;
  const Hal_Buffer<T> &operator=(const Hal_Buffer<T> &halBuffer) = delete;
  Hal_Buffer(const Hal_Buffer<T> &&halBuffer) = delete;
  Hal_Buffer<T> &operator=(Hal_Buffer<T> &&halBuffer) = delete;

  void push(T &rItem)
  {
    int err{};

    err = pthread_mutex_lock(&m_mutex);
    if (err)
    {
      throw std::runtime_error(strerror(err));
    }

    pthread_testcancel();

    m_queue.push_back(std::move(rItem));

    ++m_pushCount;

    err = pthread_cond_signal(&m_cond);
    if (err)
    {
      pthread_mutex_unlock(&m_mutex);

      throw std::runtime_error(strerror(err));
    }

    err = pthread_mutex_unlock(&m_mutex);
    if (err)
    {
      throw std::runtime_error(strerror(err));
    }
  }

  long long waitForEmpty()
  {
    int err{};
    long long count{};

    err = pthread_mutex_lock(&m_mutex);
    if (err)
    {
      throw std::runtime_error(strerror(err));
    }

    pthread_testcancel();

    while (!m_queue.empty())
    {
      err = pthread_cond_wait(&m_emptyCond, &m_mutex);
      if (err)
      {
        throw std::runtime_error(strerror(err));
      }

      pthread_testcancel();
    }

    assert(m_popCount == m_pushCount);
    count = m_popCount;

    err = pthread_mutex_unlock(&m_mutex);
    if (err)
    {
      throw std::runtime_error(strerror(err));
    }

    return count;
  }

 protected:
  T pop()
  {
    int err{};

    err = pthread_mutex_lock(&m_mutex);
    if (err)
    {
      throw std::runtime_error(strerror(err));
    }

    pthread_testcancel();

    while (m_queue.empty())
    {
      err = pthread_cond_wait(&m_cond, &m_mutex);
      if (err)
      {
        throw std::runtime_error(strerror(err));
      }

      pthread_testcancel();
    }

    T val = std::move(m_queue.front());
    m_queue.pop_front();

    ++m_popCount;

    err = pthread_cond_signal(&m_emptyCond);
    if (err)
    {
      pthread_mutex_unlock(&m_mutex);

      throw std::runtime_error(strerror(err));
    }

    err = pthread_mutex_unlock(&m_mutex);
    if (err)
    {
      throw std::runtime_error(strerror(err));
    }

    return val;  // val is local variable, hence rvalue and hence move semantic
                 // by default for efficient copy.
  }

 private:
  std::deque<T>   m_queue{};
  pthread_mutex_t m_mutex{};
  pthread_cond_t  m_cond{};
  pthread_cond_t  m_emptyCond{};
  long long       m_pushCount{};
  long long       m_popCount{};
};

#endif /* HAL_BUFFER_HPP_HAVE_SEEN */
