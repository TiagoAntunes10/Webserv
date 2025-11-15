#include "../../includes/include.hpp"

Client::Client(void) {
  client_fd_ = 0;
  poll_fd_.events = POLLIN | POLLOUT;
  poll_fd_.revents = 0;
}

Client::Client(Client &client) {
  client_fd_ = client.client_fd_;
  client_addr_ = client.client_addr_;
  poll_fd_ = client.poll_fd_;
}

Client &Client::operator=(Client &client) {
  client_fd_ = client.client_fd_;
  client_addr_ = client.client_addr_;
  poll_fd_ = client.poll_fd_;

  return (*this);
}

Client::~Client(void) { close(client_fd_); }

int Client::acceptConnection(Socket &socket) {
  client_fd_ = accept(socket.getServerFd(), (struct sockaddr *)&client_addr_,
                      &client_addr_len_);

  if (client_fd_ < 0) {
    std::cerr << RED << "Error: Client/Server Connection" << END << std::endl;
    return (client_fd_);
  }

  poll_fd_.fd = client_fd_;
  return (0);
}

// NOTE: timeout is in miliseconds
int Client::pollConnection(nfds_t num_fds, int timeout) {
  if (poll(&poll_fd_, num_fds, timeout) < 0) {
    std::cerr << RED << "Error: Polling" << END << std::endl;
    return (-1);
  }

  return (0);
}

int Client::getRevents(void) { return (poll_fd_.revents); }

int Client::getClientFd(void) { return (client_fd_); }
