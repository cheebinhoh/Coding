/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 *
 * This class implements an IO interface that
 * - read is blocking call if no data available or std::nullopt_t if
 *   end of data stream is read (example when pipe is removed).
 * - write (with rvalue) will move data if it is possible
 * - write (with lvalue) will not move but copy data
 */

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
