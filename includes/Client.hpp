#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "./HttpParser.hpp"
#include "./Response.hpp"
#include <ctime>

// TODO: Should the Client class inherite from the Response class?
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
  std::string getResponse(void);

  void setFd(int fd);
  void setRequest(http_Data &request);
  void setTimeLastCom(void);
  void setStatusCode(int status);

private:
  int fd_;
  int status_code_;
  http_Data request_;
  Response response_;
  time_t t_last_com_;
};

#endif
