#ifndef HAL_BUFFER_HPP_HAVE_SEEN

#define HAL_BUFFER_HPP_HAVE_SEEN

#include <cassert>
#include <deque>
#include <pthread.h>

template <typename T>
class Hal_Buffer
{
  public:
    Hal_Buffer() {

    }

    void push(T item) {
      int err {};

      err = pthread_mutex_lock(&m_mutex);
      assert(err);
      m_queue.push_back(item);
   
      err = pthread_cond_signal(&m_cond);
      assert(err);
      err = pthread_mutex_unlock(&m_mutex);
      assert(err);
    }

  protected:
    T pop() {
      int err {};

      err = pthread_mutex_lock(&m_mutex);
      
      while (m_queue.empty()) {
         pthread_cond_wait(&m_cond, &m_mutex);
      }

      T val = m_queue.front();
      m_queue.pop_front();

      err = pthread_mutex_unlock(&m_mutex);

      return val;
    }

  private:
    std::deque<T>           m_queue {};

    pthread_mutex_t m_mutex {};
    pthread_cond_t m_cond {};
};

#endif /* HAL_BUFFER_HPP_HAVE_SEEN */
