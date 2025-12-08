#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "./HttpParser.hpp"
#include <ctime>

class Client {
public:
  Client(void);
  Client(int fd);
  Client(Client const &client);
  Client &operator=(Client const &client);
  ~Client(void);

  int getFd(void);
  http_Data getRequest(void);
  time_t getTimeLastCom(void);

  void setFd(int fd);
  void setRequest(http_Data &request);
  void setTimeLastCom(void);

private:
  int fd_;
  http_Data request_;
  time_t t_last_com_;
};

#endif
