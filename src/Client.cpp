#include "../includes/Webserver.hpp"

// TODO: Change implemenation to use the select() function
Client::Client(void) {
  backlog_ = 0;
  highsock_ = 0;
}

Client::Client(int backlog, Socket &socket) {
  backlog_ = backlog;
  highsock_ = socket.getServerFd();
}

Client::Client(Client &client) {
  client_fd_ = client.client_fd_;
  highsock_ = client.highsock_;
}

Client &Client::operator=(Client &client) {
  client_fd_ = client.client_fd_;
  highsock_ = client.highsock_;

  return (*this);
}

Client::~Client(void) {
  iterator it = client_fd_.begin();
  iterator end = client_fd_.end();

  for (; it != end; it++) {
    if (FD_ISSET(*it, &socks_))
      close(*it);
  }
}

void Client::initClient(int backlog, Socket &socket) {
  backlog_ = backlog;
  highsock_ = socket.getServerFd();
}

void Client::buildFdList(Socket &socket) {
  iterator it = client_fd_.begin();
  iterator end = client_fd_.end();

  FD_ZERO(&socks_);

  FD_SET(socket.getServerFd(), &socks_);

  for (; it != end; it++) {
    FD_SET(*it, &socks_);
    if (*it > highsock_)
      highsock_ = *it;
  }
}

void Client::createNewConnection(Socket &socket) {
  int connection;

  connection = accept(socket.getServerFd(), NULL, NULL);
  if (connection < 0) {
    std::cerr << RED << "Error: Client/Server Connection" << END << std::endl;
    exit(EXIT_FAILURE);
  }

  set_non_blocking(connection);

  if ((int)client_fd_.size() < backlog_) {
    client_fd_.push_back(connection);
    std::cout << GREEN << "Connection accepted: FD=" << connection
              << "; Slot=" << client_fd_.size() << END << std::endl;
  } else {
    // TODO: Send a message to the client
    std::cerr << YELLOW << "No space for new clients!" << END << std::endl;
    close(connection);
  }
}

void Client::waitConnections(Socket &socket) {
  int read_socks;
  struct timeval timeout;

  timeout.tv_sec = 10;
  timeout.tv_usec = 0;

  read_socks = select(highsock_ + 1, &socks_, NULL, NULL, &timeout);

  if (read_socks < 0) {
    std::cerr << YELLOW << "Error: selecting connections" << END << std::endl;
    exit(EXIT_FAILURE);
  } else if (read_socks == 0)
    std::cout << CYAN << "..." << END << std::endl;
  else
    getMessages(socket);
}

void Client::getMessages(Socket &socket) {
  // TODO: Refactor the use of the buffer
  char *buff = new char[BUFF_SIZE];
  unsigned short stat;
  http_Data dataReq;

  if (FD_ISSET(socket.getServerFd(), &socks_))
    createNewConnection(socket);

  iterator it = client_fd_.begin();
  iterator end = client_fd_.end();

  for (; it != end; it++)
    if (FD_ISSET(*it, &socks_)) {
      ssize_t bytes_received = recv(*it, buff, BUFF_SIZE, 0);

      if (bytes_received < 0) {
        if (buff)
          delete[] buff;
        std::cerr << RED << "Error: Receiving message" << END << std::endl;
        exit(EXIT_FAILURE);
      }

      if (bytes_received > 0) {
        if (!(stat = HttpParser::parseHeader(buff, dataReq))) {
          std::cerr << "❌ Invalid HTTP request" << std::endl;
          if (buff)
            delete[] buff;
          exit(EXIT_FAILURE);
        }
      }

      if (stat) {
        requests_.insert(std::pair<int, http_Data>(*it, dataReq));
        std::cout << BLUE << "Request from fd=" << *it << END << std::endl;
        std::cout << requests_.find(*it)->second << std::endl;
      }
    }

  if (buff)
    delete[] buff;
}
