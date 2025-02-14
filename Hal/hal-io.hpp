#ifndef HAL_IO_HPP_HAVE_SEEN

#define HAL_IO_HPP_HAVE_SEEN

#include <optional>

template <typename T> class Hal_Io {
public:
  virtual ~Hal_Io() = default;
  virtual std::optional<T> read() = 0;
  virtual void write(T &rItem) = 0;
  virtual void write(T &&rItem) = 0;
};

#endif /* HAL_IO_HPP_HAVE_SEEN */
