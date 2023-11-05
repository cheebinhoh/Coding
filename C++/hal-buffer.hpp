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
      int err {};

      err = pthread_mutex_init(&m_mutex, NULL);
      if (err) {
         throw std::runtime_error(strerror(err));
      }

      err = pthread_cond_init(&m_cond, NULL);
      if (err) {
         throw std::runtime_error(strerror(err));
      }
    }

    void push(T item) {
      int err {};

      err = pthread_mutex_lock(&m_mutex);
      if (err) {
         throw std::runtime_error(strerror(err));
      }

      m_queue.push_back(item);
   
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

  protected:
    T pop() {
      int err {};

      err = pthread_mutex_lock(&m_mutex);
      if (err) {
         throw std::runtime_error(strerror(err));
      }
      
      while (m_queue.empty()) {
         err = pthread_cond_wait(&m_cond, &m_mutex);
         if (err) {
             throw std::runtime_error(strerror(err));
         }
      }

      T val = m_queue.front();
      m_queue.pop_front();

      err = pthread_mutex_unlock(&m_mutex);
      if (err) {
         throw std::runtime_error(strerror(err));
      }
 
      return val;
    }

  private:
    std::deque<T>           m_queue {};

    pthread_mutex_t m_mutex {};
    pthread_cond_t m_cond {};
};

#endif /* HAL_BUFFER_HPP_HAVE_SEEN */
