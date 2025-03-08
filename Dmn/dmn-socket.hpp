/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 */

#ifndef DMN_SOCKET_HPP_HAVE_SEEN

#define DMN_SOCKET_HPP_HAVE_SEEN

#include "dmn-io.hpp"

#include <optional>
#include <string>
#include <string_view>

namespace Dmn {

class Dmn_Socket : public Dmn_Io<std::string> {
public:
  Dmn_Socket(std::string_view ip4, int portno, bool writeOnly = false);
  ~Dmn_Socket();
  std::optional<std::string> read() override;
  void write(std::string &item) override;
  void write(std::string &&item) override;

private:
  std::string m_ip4{};
  int m_portno{};
  bool m_writeOnly{};

  int m_fd{-1};
};

} // namespace Dmn

#endif /* DMN_SOCKET_HPP_HAVE_SEEN */
