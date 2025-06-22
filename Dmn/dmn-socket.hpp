/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 *
 * @file dmn-socket.hpp
 * @brief The header file for dmn-socket which implements ocket IO interface.
 */

#ifndef DMN_SOCKET_HPP_

#define DMN_SOCKET_HPP_

#include <optional>
#include <string>
#include <string_view>

#include "dmn-io.hpp"

namespace dmn {

class Dmn_Socket : public Dmn_Io<std::string> {
public:
  Dmn_Socket(std::string_view ip4, int port_no, bool write_only = false);
  virtual ~Dmn_Socket();

  Dmn_Socket(const Dmn_Socket &obj) = delete;
  const Dmn_Socket &operator=(const Dmn_Socket &obj) = delete;
  Dmn_Socket(Dmn_Socket &&obj) = delete;
  Dmn_Socket &operator=(Dmn_Socket &&obj) = delete;

  std::optional<std::string> read() override;
  void write(std::string &item) override;
  void write(std::string &&item) override;

private:
  /**
   * data members for constructor to instantiate the object.
   */
  std::string m_ip4{};
  int m_port_no{};
  bool m_write_only{};

  /**
   * data members for internal logic.
   */
  int m_fd{-1};
};

} // namespace dmn

#endif // DMN_SOCKET_HPP_
