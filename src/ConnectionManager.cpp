#include "../includes/Webserver.hpp"
#include <sys/select.h>

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
// NOTE: FDs are closed here to prevent problems during vector memory management
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
    Logger::getLogger().consoleMsg(stderr, RED,
                                   "Error: Client/Server Connection");
    return;
  }

  if (!set_non_blocking(connection)) {
    Logger::getLogger().consoleMsg(
        stderr, RED, "Error: Setting FD=%d as non-blocking", connection);
    return;
  }

  Client client;

  if ((int)clients_.size() < backlog_) {
    clients_.push_back(client);
    clients_.back().setFd(connection);
    Logger::getLogger().consoleMsg(stdout, GREEN,
                                   "Connection accepted: FD=%d; Slot=%d",
                                   connection, clients_.size());
  } else {
    // TODO: Send a message to the client
    Logger::getLogger().consoleMsg(stderr, YELLOW, "No space for new clients!");
    close(connection);
  }

  clients_.back().setTimeLastCom();
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
    Logger::getLogger().consoleMsg(stderr, RED, "Error: selecting connections");
    return;
  } else if (socks == 0) {
    Logger::getLogger().consoleMsg(stdout, CYAN, "...");
  } else {
    if (FD_ISSET(socket.getServerFd(), &read_cpy))
      createNewConnection(socket);

    for (iterator it = clients_.begin(); it != clients_.end(); it++) {
      if (FD_ISSET(it->getFd(), &read_cpy))
        getMessages(it);
      else if (FD_ISSET(it->getFd(), &write_cpy)) {
        sendResponse(it);
        continue;
      }
    }
  }

  checkTimeout();
}

void ConnectionManager::getMessages(iterator &it) {
  // TODO: Refactor the use of the buffer
  char *buff = new char[BUFF_SIZE];
  unsigned short stat;
  http_Data dataReq;

  ssize_t bytes_received = recv(it->getFd(), buff, BUFF_SIZE, 0);

  if (bytes_received < 0) {
    if (buff)
      delete[] buff;
    Logger::getLogger().consoleMsg(stderr, RED, "Error: Receiving message");
    return;
  } else if (bytes_received > 0) {
    stat = HttpParser::parseHeader(buff, dataReq);
    it->setStatusCode(stat);

    if (stat == 0) {
      Logger::getLogger().consoleMsg(stderr, RED, "❌ Invalid HTTP request");
      if (buff)
        delete[] buff;

      return;
    } else {
      it->setRequest(dataReq);

      Logger::getLogger().consoleMsg(stdout, BLUE, "Request from FD=%d",
                                     it->getFd());

      // NOTE: Strange types (e.g. http_Data) cannot be passed to the
      // variadic argument function in the Logger class
      std::cout << it->getRequest() << std::endl;

      // NOTE: Fd stops waiting to read and starts waiting to write
      FD_SET(it->getFd(), &write_socks_);
      FD_CLR(it->getFd(), &read_socks_);

      it->setTimeLastCom();
    }
  }

  if (buff)
    delete[] buff;
}

void ConnectionManager::checkTimeout(void) {
  for (iterator it = clients_.begin(); it != clients_.end(); it++) {
    if (it->getTimeLastCom() && (time(NULL) - it->getTimeLastCom()) > TIMEOUT) {
      Logger::getLogger().consoleMsg(
          stdout, BLUE, "Connection FD=%d has been closed for inactivity",
          it->getFd());

      if (FD_ISSET(it->getFd(), &read_socks_))
        FD_CLR(it->getFd(), &read_socks_);
      else if (FD_ISSET(it->getFd(), &write_socks_))
        FD_CLR(it->getFd(), &write_socks_);

      // TODO: Check if this works
      if (highsock_ == it->getFd())
        highsock_--;

      // NOTE: This is necessary because closing the clients' fd cannot be done
      // in the destructor (problems with vector resizing)
      close(it->getFd());

      clients_.erase(it);
      return;
    }
  }
}

void ConnectionManager::sendResponse(iterator &it) {
  std::string response = it->getResponse();

  if (send(it->getFd(), response.c_str(), response.length(), 0) < 0) {
    Logger::getLogger().consoleMsg(stderr, RED,
                                   "Error: Sending message to client");
  }

  // NOTE: Fd stops waiting to write and starts waiting to read
  FD_SET(it->getFd(), &read_socks_);
  FD_CLR(it->getFd(), &write_socks_);

  it->setTimeLastCom();
}
