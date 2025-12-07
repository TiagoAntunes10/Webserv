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

  FD_SET(socket.getServerFd(), &read_socks_);

  for (; it != end; it++) {
    if (!FD_ISSET(*it, &read_socks_) && !FD_ISSET(*it, &write_socks_))
      FD_SET(*it, &read_socks_);
    if (*it > highsock_)
      highsock_ = *it;
  }
}

void Client::createNewConnection(Socket &socket) {
  int connection;

  connection = accept(socket.getServerFd(), NULL, NULL);
  if (connection < 0) {
    Logger::consoleMsg(std::cerr, RED, "%s", "Error: Client/Server Connection");
    exit(EXIT_FAILURE);
  }

  if (!set_non_blocking(connection)) {
    Logger::consoleMsg(std::cerr, RED, "%s%d%s",
                       "Error: Setting fd=", connection, " as non-blocking");
    return;
  }

  if ((int)client_fd_.size() < backlog_) {
    client_fd_.push_back(connection);
    Logger::consoleMsg(std::cout, GREEN, "%s%d%s%d",
                       "Connection accepted: FD=", connection,
                       "; Slot=", client_fd_.size());
  } else {
    // TODO: Send a message to the client
    Logger::consoleMsg(std::cerr, YELLOW, "%s", "No space for new clients!");
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
    Logger::consoleMsg(std::cerr, YELLOW, "%s", "Error: selecting connections");
    exit(EXIT_FAILURE);
  } else if (read_socks == 0)
    Logger::consoleMsg(std::cout, CYAN, "%s", "...");
  else
    getMessages(socket);
}

void Client::getMessages(Socket &socket) {
  // TODO: Refactor the use of the buffer
  char *buff = new char[BUFF_SIZE];
  unsigned short stat;
  http_Data dataReq;

  if (FD_ISSET(socket.getServerFd(), &read_socks_))
    createNewConnection(socket);

  iterator it = client_fd_.begin();
  iterator end = client_fd_.end();

  for (; it != end; it++) {
    if (FD_ISSET(*it, &read_socks_)) {
      ssize_t bytes_received = recv(*it, buff, BUFF_SIZE, 0);

      if (bytes_received < 0) {
        if (buff)
          delete[] buff;
        Logger::consoleMsg(std::cerr, RED, "%s", "Error: Receiving message");
      } else if (bytes_received > 0) {
        stat = HttpParser::parseHeader(buff, dataReq);

        if (stat == 0) {
          Logger::consoleMsg(std::cerr, RED, "%s", "❌ Invalid HTTP request");
          if (buff)
            delete[] buff;
          exit(EXIT_FAILURE);
        } else {
          requests_.insert(std::pair<int, http_Data>(*it, dataReq));
          Logger::consoleMsg(std::cout, BLUE, "%s%d", "Request from fd=", *it);

          // NOTE: Strange types (e.g. http_Data) cannot be passed to the
          // variadic argument function in the Logger class
          std::cout << requests_.find(*it)->second << std::endl;

          FD_SET(*it, &write_socks_);
          FD_CLR(*it, &read_socks_);
        }
      }
    }
  }

  if (buff)
    delete[] buff;
}
