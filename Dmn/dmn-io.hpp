/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 *
 * This class implements an IO interface that
 * - read is blocking call if no data available or std::nullopt_t if
 *   end of data stream is read (example when pipe is removed) or
 *   end of file, or no udp packet for next read, no data of specific
 *   topic in kafka broker for next read.
 * - write (with rvalue) will move data if it is possible
 * - write (with lvalue) will not move but copy data
 */

#ifndef DMN_IO_HPP_HAVE_SEEN

#define DMN_IO_HPP_HAVE_SEEN

#include <optional>

namespace Dmn {

template <typename T> class Dmn_Io {
public:
  virtual ~Dmn_Io() = default;
  virtual std::optional<T> read() = 0;
  virtual void write(T &item) = 0;
  virtual void write(T &&item) = 0;
};

} /* End of namespace Dmn */

#endif /* End of macro DMN_IO_HPP_HAVE_SEEN */
