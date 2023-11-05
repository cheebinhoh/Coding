#ifndef HAL_BUFFER_HPP_HAVE_SEEN

#define HAL_BUFFER_HPP_HAVE_SEEN

#include <cassert>
#include <deque>
#include <condition_variable>
#include <mutex>

template <typename T>
class Hal_Buffer
{
  public:
    void push(T item) {
      std::unique_lock<std::mutex> lck(m_mutex);
      m_queue.push_back(item);
   
      m_cond.notify_all();
    }

  protected:
    T pop() {

      std::unique_lock<std::mutex> lck(m_mutex);
      
      while (m_queue.empty()) {
          m_cond.wait(lck);
      }

      T val = m_queue.front();
      m_queue.pop_front();

      return val;
    }

  private:
    std::deque<T>           m_queue {};
    std::mutex              m_mutex {};
    std::condition_variable m_cond {};
};

#endif /* HAL_BUFFER_HPP_HAVE_SEEN */
