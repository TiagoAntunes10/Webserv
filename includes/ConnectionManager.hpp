#ifndef CONNECT_HPP
#define CONNECT_HPP

#include "./Client.hpp"
#include "./HttpParser.hpp"
#include "./Socket.hpp"
#include <iostream>
#include <poll.h>
#include <vector>

class ConnectionManager {
public:
  typedef std::vector<Client>::iterator iterator;

  ConnectionManager(void);
  ConnectionManager(int backlog, Socket &socket);
  ConnectionManager(ConnectionManager &manager);
  ConnectionManager &operator=(ConnectionManager &manager);
  ~ConnectionManager(void);

  void initManager(int backlog, Socket &socket);
  void buildFdList(Socket &socket);
  void createNewConnection(Socket &socket);
  void waitConnections(Socket &socket);
  void getMessages(iterator &it);
  void checkTimeout(void);
  void sendResponse(iterator &it);

private:
  std::vector<Client> clients_;
  int backlog_;
  fd_set read_socks_;
  fd_set write_socks_;
  int highsock_;
};

#endif
