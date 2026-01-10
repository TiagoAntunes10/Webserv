#include "../includes/Webserver.hpp"

int main(void) {
  Socket socket(AF_INET, SOCK_STREAM, 0);

  Logger::getLogger().initLogger();

  // TODO: Change values with information from the config file
  socket.bindSocket(AF_INET, htons(PORT), htonl(INADDR_ANY));

  // TODO: Change values with information from the config file
  socket.listenSocket(BACKLOG);

  Logger::getLogger().consoleMsg(stdout, GREEN, "Listening on port %d...",
                                 PORT);

  // TODO: Change values with information from the config file
  ConnectionManager manager(BACKLOG, socket);

  while (1) {
    manager.buildFdList(socket);
    manager.waitConnections(socket);
  }
}
