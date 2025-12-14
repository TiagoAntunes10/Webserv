#include "../includes/Webserver.hpp"

int main(void) {
  Socket socket(AF_INET, SOCK_STREAM, 0);

  Logger::getLogger();
  Logger::getLogger().initLogger();

  socket.bindSocket(AF_INET, htons(PORT), htonl(INADDR_ANY));

  socket.listenSocket(BACKLOG);

  Logger::getLogger().consoleMsg(stdout, GREEN, "Listening on port %d...",
                                 PORT);

  ConnectionManager manager(BACKLOG, socket);

  std::string status = "HTTP/1.1 200 OK\r\n";
  std::string header = "Content-Type: Text/html\r\n\r\n";
  std::string body = "{\
	<html>\
	<body>\
	<h1>Hello, World!</h1>\
	</body>\
	</html>\
	  }";
  std::string msg = status + header + body;

  while (1) {
    manager.buildFdList(socket);
    manager.waitConnections(socket);

    // std::string response = build_response(stat, dataReq);
    //
    // if (response.empty()) {
    //   std::cerr << "Error: Enable to build response" << std::endl;
    //   return (EXIT_FAILURE);
    // }
    //
    // // TODO: Should this be a method in the client class?
    // if ((client.getRevents() & POLLOUT) == POLLOUT) {
    //   if (send(client.getClientFd(), msg.c_str(), msg.length(), 0) < 0) {
    //     delete[] buff;
    //     std::cerr << "Error: Send failed" << std::endl;
    //     return (EXIT_FAILURE);
    //   }
  }
}
