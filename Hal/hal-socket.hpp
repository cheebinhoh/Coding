/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 */

#ifndef HAL_SOCKET_HPP_HAVE_SEEN

#define HAL_SOCKET_HPP_HAVE_SEEN

#include "hal-io.hpp"

#include <optional>
#include <string>
#include <string_view>

namespace Hal {

class Hal_Socket : public Hal_Io<std::string> {
public:
  Hal_Socket(std::string_view ip4, int portno, bool writeOnly = false);
  ~Hal_Socket();
  std::optional<std::string> read();
  void write(std::string &item);
  void write(std::string &&item);

private:
  std::string m_ip4{};
  int m_portno{};
  bool m_writeOnly{};
  int m_fd{-1};
};

} // namespace Hal

#endif /* HAL_SOCKET_HPP_HAVE_SEEN */
