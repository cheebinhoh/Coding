#ifndef HAL_BUFFER_HPP_HAVE_SEEN

#define HAL_BUFFER_HPP_HAVE_SEEN

#include <algorithm>
#include <cassert>
#include <deque>
#include <pthread.h>

using std::chrono::high_resolution_clock;
using std::chrono::system_clock;

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

    ~Hal_Buffer() {
       std::cout << "queue peak size: " << m_queue_peak_size << "\n";
       std::cout << "last pop to first push time (nanoseconds): " << 
         std::chrono::duration_cast<std::chrono::nanoseconds>(m_last_pop_time - m_first_push_time).count() << "\n";
    }

    void push(T& item) {
      int err {};

      err = pthread_mutex_lock(&m_mutex);
      if (err) {
         throw std::runtime_error(strerror(err));
      }

      std::call_once(m_first_push_flag, [this]() {
        this->m_first_push_time = system_clock::now();
      });

      m_queue.push_back(std::move(item));

      m_queue_peak_size = std::max(m_queue_peak_size,
                                   m_queue.size());

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

      T val = std::move(m_queue.front());
      m_queue.pop_front();

      this->m_last_pop_time = system_clock::now();

      err = pthread_mutex_unlock(&m_mutex);
      if (err) {
         throw std::runtime_error(strerror(err));
      }
 
      return std::move(val);
    }

  private:
    std::deque<T> m_queue {};
    pthread_mutex_t m_mutex {};
    pthread_cond_t m_cond {};

    // statistics
    size_t m_queue_peak_size {};
 
    std::once_flag m_first_push_flag {};
    system_clock::time_point m_first_push_time {};
    system_clock::time_point m_last_pop_time {};
};

#endif /* HAL_BUFFER_HPP_HAVE_SEEN */
