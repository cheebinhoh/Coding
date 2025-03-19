/**
 * Copyright © 2025 Chee Bin HOH. All rights reserved.
 */

#include "dmn-socket.hpp"

#include <cstdio>
#include <cstring>
#include <optional>
#include <stdexcept>

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

namespace Dmn {

Dmn_Socket::Dmn_Socket(std::string_view ip4, int portno, bool writeOnly)
    : m_ip4{ip4}, m_portno{portno}, m_writeOnly{writeOnly} {
  constexpr int broadcast{1};
  struct sockaddr_in servaddr;
  int type{SOCK_DGRAM};

  if ((m_fd = socket(AF_INET, type, 0)) < 0) {
    throw std::runtime_error("Error in read socket: " +
                             std::string(strerror(errno)));
  }

  memset(&servaddr, 0, sizeof(servaddr));

  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(m_portno);
  if ("" == m_ip4) {
    servaddr.sin_addr.s_addr = INADDR_ANY;
  } else {
    inet_pton(AF_INET, m_ip4.c_str(), &servaddr.sin_addr);
  }

  if (setsockopt(m_fd, SOL_SOCKET, SO_BROADCAST, &broadcast,
                 sizeof(broadcast)) < 0) {
    throw std::runtime_error("Error in setsockopt: SO_SOCKET, SO_BROADCAST: " +
                             std::string(strerror(errno)));
  }

  if (!m_writeOnly &&
      bind(m_fd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
    throw std::runtime_error("Error in bind(" + std::to_string(m_portno) +
                             "): " + std::string(strerror(errno)));
  }
}

Dmn_Socket::~Dmn_Socket() {
  if (-1 != m_fd) {
    close(m_fd);
  }
}

std::optional<std::string> Dmn_Socket::read() {
  char buf[BUFSIZ]; /* FIXME: need to scale it as size of DMesgPb */
  ssize_t nRead{};

  if ((nRead = recv(m_fd, buf, sizeof(buf), MSG_WAITALL)) < 0) {
    throw std::runtime_error("Error in recv: " + std::string(strerror(errno)));
  } else if (nRead == 0) {
    return {};
  }

  std::string string(buf, nRead);

  return string;
}

void Dmn_Socket::write(std::string &item) {
  const char *buf{item.c_str()};
  size_t nRead{item.size()};
  size_t nWrite{};

  /* FIXME: it might be effectiveto store socket_addr as member value per object
   */
  struct sockaddr_in servaddr;
  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(m_portno);
  if ("" == m_ip4) {
    servaddr.sin_addr.s_addr = INADDR_ANY;
  } else {
    inet_pton(AF_INET, m_ip4.c_str(), &servaddr.sin_addr);
  }

  nWrite = sendto(m_fd, buf, nRead, 0, /* FIXME: temporary for macOS */
                  (const struct sockaddr *)&servaddr, sizeof(servaddr));
  if (nWrite != nRead) {
    throw std::runtime_error("Error in sendto: " +
                             std::string(strerror(errno)));
  }
}

void Dmn_Socket::write(std::string &&item) {
  std::string moveString = std::move_if_noexcept(item);

  write(moveString);
}

} // namespace Dmn
