#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "./HttpParser.hpp"
#include "./Socket.hpp"
#include <iostream>
#include <map>
#include <poll.h>
#include <vector>

class Client {
public:
  typedef std::vector<int>::iterator iterator;

  Client(void);
  Client(int backlog, Socket &socket);
  Client(Client &client);
  Client &operator=(Client &client);
  ~Client(void);
  void initClient(int backlog, Socket &socket);
  void buildFdList(Socket &socket);
  void createNewConnection(Socket &socket);
  void waitConnections(Socket &socket);
  void getMessages(Socket &socket);

private:
  std::vector<int> client_fd_;
  std::map<int, http_Data> requests_;
  int backlog_;
  fd_set socks_;
  int highsock_;
};

#endif
