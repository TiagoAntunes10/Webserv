#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "./Socket.hpp"
#include <poll.h>

class Client {
public:
  Client(void);
  Client(Client &client);
  Client &operator=(Client &client);
  ~Client(void);
  int acceptConnection(Socket &socket);
  // NOTE: timeout is in miliseconds
  int pollConnection(nfds_t num_fds, int timeout);
  int getRevents(void);
  int getClientFd(void);

private:
  int client_fd_;
  struct sockaddr_in client_addr_;
  socklen_t client_addr_len_ = sizeof(client_addr_);
  struct pollfd poll_fd_;
};

#endif
