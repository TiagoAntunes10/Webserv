#include "../includes/Webserver.hpp"

Client::Client(void) {
  fd_ = 0;
  t_last_com_ = 0;
}

Client::Client(int fd) {
  fd_ = fd;
  t_last_com_ = 0;
}

Client::Client(Client const &client) {
  fd_ = client.fd_;
  request_ = client.request_;
  if (client.t_last_com_)
    t_last_com_ = client.t_last_com_;
}

Client &Client::operator=(Client const &client) {
  fd_ = client.fd_;
  request_ = client.request_;
  if (client.t_last_com_)
    t_last_com_ = client.t_last_com_;

  return (*this);
}

// NOTE: If the fd is closed here, the fd will be closed during vector
// operations
Client::~Client(void) { return; }

int Client::getFd(void) { return (fd_); }

http_Data Client::getRequest(void) { return (request_); }

time_t Client::getTimeLastCom(void) { return (t_last_com_); }

std::string Client::getResponse(void) {
  std::string response = response_.buildResponse(status_code_, request_);

  return (response);
}

void Client::setFd(int fd) { fd_ = fd; }

void Client::setRequest(http_Data &request) { request_ = request; }

void Client::setTimeLastCom(void) { t_last_com_ = time(NULL); }

void Client::setStatusCode(int status) { status_code_ = status; }
