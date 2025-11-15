#include "../../includes/include.hpp"

Socket::Socket(void) { server_fd_ = 0; }

// TODO: Create utils directory with function to write error messages
Socket::Socket(int domain, int type, int protocol) {
  server_fd_ = socket(domain, type, protocol);

  if (server_fd_ < 0) {
    std::cerr << RED << "Error: socket creation" << END << std::endl;
    exit(EXIT_FAILURE);
  }
}

Socket::Socket(Socket &sock) {
  server_fd_ = sock.server_fd_;
  server_addr_ = sock.server_addr_;
}

Socket &Socket::operator=(Socket &sock) {
  server_fd_ = sock.server_fd_;
  server_addr_ = sock.server_addr_;

  return (*this);
}

Socket::~Socket(void) { close(server_fd_); }

void Socket::createSocket(int domain, int type, int protocol) {
  server_fd_ = socket(domain, type, protocol);

  if (server_fd_ < 0) {
    std::cerr << RED << "Error: socket creation" << END << std::endl;
    exit(EXIT_FAILURE);
  }
}

void Socket::bindSocket(sa_family_t sin_family, in_port_t sin_port,
                        in_addr_t sin_addr) {
  server_addr_.sin_family = sin_family;
  server_addr_.sin_port = sin_port;
  server_addr_.sin_addr.s_addr = sin_addr;

  if (bind(server_fd_, (struct sockaddr *)&server_addr_, server_addr_len_) <
      0) {
    std::cerr << RED << "Error: binding socket" << END << std::endl;
    exit(EXIT_FAILURE);
  }
}

void Socket::listenSocket(int backlog) {
  if (listen(server_fd_, backlog) < 0) {
    std::cerr << RED << "Error: binding socket" << END << std::endl;
    exit(EXIT_FAILURE);
  }
}

int Socket::getServerFd(void) { return (server_fd_); }
