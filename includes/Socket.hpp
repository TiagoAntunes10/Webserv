#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <arpa/inet.h>
#include <sys/socket.h>

class Socket {
public:
  Socket(void);
  Socket(int domain, int type, int protocol);
  Socket(Socket &sock);
  Socket &operator=(Socket &sock);
  ~Socket(void);
  void createSocket(int domain, int type, int protocol);
  void bindSocket(sa_family_t sin_family, in_port_t sin_port,
                  in_addr_t sin_addr);
  void listenSocket(int backlog);
  int getServerFd(void);

private:
  int server_fd_;
  struct sockaddr_in server_addr_;
  socklen_t server_addr_len_;
};

#endif
