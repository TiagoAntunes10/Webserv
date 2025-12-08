#include "../includes/Webserver.hpp"
#include <cstdlib>

ConnectionManager::ConnectionManager(void) {
  backlog_ = 0;
  highsock_ = 0;
}

ConnectionManager::ConnectionManager(int backlog, Socket &socket) {
  backlog_ = backlog;
  highsock_ = socket.getServerFd();
}

ConnectionManager::ConnectionManager(ConnectionManager &manager) {
  clients_ = manager.clients_;
  backlog_ = manager.backlog_;
  read_socks_ = manager.read_socks_;
  write_socks_ = manager.write_socks_;
  highsock_ = manager.highsock_;
}

ConnectionManager &ConnectionManager::operator=(ConnectionManager &manager) {
  clients_ = manager.clients_;
  backlog_ = manager.backlog_;
  read_socks_ = manager.read_socks_;
  write_socks_ = manager.write_socks_;
  highsock_ = manager.highsock_;

  return (*this);
}

// TODO: Confirm there are no fds that remain open
ConnectionManager::~ConnectionManager(void) {
  FD_ZERO(&read_socks_);
  FD_ZERO(&write_socks_);

  for (iterator it = clients_.begin(); it != clients_.end(); it++)
    close(it->getFd());
}

void ConnectionManager::initManager(int backlog, Socket &socket) {
  backlog_ = backlog;
  highsock_ = socket.getServerFd();
}

void ConnectionManager::buildFdList(Socket &socket) {
  if (!FD_ISSET(socket.getServerFd(), &read_socks_))
    FD_SET(socket.getServerFd(), &read_socks_);

  for (iterator it = clients_.begin(); it != clients_.end(); it++) {
    if (!FD_ISSET(it->getFd(), &read_socks_) &&
        !FD_ISSET(it->getFd(), &write_socks_))
      FD_SET(it->getFd(), &read_socks_);

    if (it->getFd() > highsock_)
      highsock_ = it->getFd();
  }
}

void ConnectionManager::createNewConnection(Socket &socket) {
  int connection;

  connection = accept(socket.getServerFd(), NULL, NULL);
  if (connection < 0) {
    Logger::consoleMsg(std::cerr, RED, "%s", "Error: Client/Server Connection");
    return;
  }

  if (!set_non_blocking(connection)) {
    Logger::consoleMsg(std::cerr, RED, "%s%d%s",
                       "Error: Setting fd=", connection, " as non-blocking");
    return;
  }

  Client client;

  if ((int)clients_.size() < backlog_) {
    clients_.push_back(client);
    clients_.back().setFd(connection);
    Logger::consoleMsg(std::cout, GREEN, "%s%d%s%d",
                       "Connection accepted: FD=", connection,
                       "; Slot=", clients_.size());
  } else {
    // TODO: Send a message to the client
    Logger::consoleMsg(std::cerr, YELLOW, "%s", "No space for new clients!");
    close(connection);
  }
}

void ConnectionManager::waitConnections(Socket &socket) {
  int socks;
  struct timeval timeout;
  fd_set read_cpy = read_socks_;
  fd_set write_cpy = write_socks_;

  timeout.tv_sec = TIMEOUT;
  timeout.tv_usec = 0;

  socks = select(highsock_ + 1, &read_cpy, &write_cpy, NULL, &timeout);

  if (socks < 0) {
    Logger::consoleMsg(std::cerr, RED, "%s", "Error: selecting connections");
    return;
  } else if (socks == 0)
    Logger::consoleMsg(std::cout, CYAN, "%s", "...");
  else {
    if (FD_ISSET(socket.getServerFd(), &read_cpy))
      createNewConnection(socket);

    for (iterator it = clients_.begin(); it != clients_.end(); it++) {
      if (FD_ISSET(it->getFd(), &read_cpy))
        getMessages(it);
      else if (FD_ISSET(it->getFd(), &write_cpy)) {
        // TODO: Do something
        continue;
      }
    }
  }

  checkTimeout();
}

void ConnectionManager::getMessages(iterator it) {
  // TODO: Refactor the use of the buffer
  char *buff = new char[BUFF_SIZE];
  unsigned short stat;
  http_Data dataReq;

  ssize_t bytes_received = recv(it->getFd(), buff, BUFF_SIZE, 0);

  if (bytes_received < 0) {
    if (buff)
      delete[] buff;
    Logger::consoleMsg(std::cerr, RED, "%s", "Error: Receiving message");
    return;
  } else if (bytes_received > 0) {
    stat = HttpParser::parseHeader(buff, dataReq);

    if (stat == 0) {
      Logger::consoleMsg(std::cerr, RED, "%s", "❌ Invalid HTTP request");
      if (buff)
        delete[] buff;

      return;
    } else {
      it->setRequest(dataReq);

      Logger::consoleMsg(std::cout, BLUE, "%s%d",
                         "Request from fd=", it->getFd());

      // NOTE: Strange types (e.g. http_Data) cannot be passed to the
      // variadic argument function in the Logger class
      std::cout << it->getRequest() << std::endl;

      FD_SET(it->getFd(), &write_socks_);
      FD_CLR(it->getFd(), &read_socks_);
    }
  }

  it->setTimeLastCom();

  if (buff)
    delete[] buff;
}

void ConnectionManager::checkTimeout(void) {
  for (iterator it = clients_.begin(); it != clients_.end(); it++) {
    if (it->getTimeLastCom() && (time(NULL) - it->getTimeLastCom()) > TIMEOUT) {
      Logger::consoleMsg(std::cout, BLUE, "%s%d%s",
                         "Connection FD=", it->getFd(),
                         " has been closed for inactivity");

      if (FD_ISSET(it->getFd(), &read_socks_))
        FD_CLR(it->getFd(), &read_socks_);
      else if (FD_ISSET(it->getFd(), &write_socks_))
        FD_CLR(it->getFd(), &write_socks_);

      // TODO: Check if this works
      if (highsock_ == it->getFd())
        highsock_--;

      // NOTE: This is necessary because closing the clients' fd cannot be done
      // in the destructor
      close(it->getFd());

      clients_.erase(it);
      return;
    }
  }
}
