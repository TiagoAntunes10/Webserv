#include "../includes/include.hpp"

int main(void) {
  Socket socket(AF_INET, SOCK_STREAM, 0);

  socket.bindSocket(AF_INET, htons(PORT), INADDR_ANY);

  socket.listenSocket(BACKLOG);

  std::cout << RED << "Listening on port " << PORT << "..." << END << std::endl;

  Client client;

  char *buff = new char(BUFF_SIZE);
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

  while ((client.getRevents() & POLLHUP) != POLLHUP) {
    if (client.acceptConnection(socket) < 0)
      delete[] buff;

    // NOTE: Timeout is in miliseconds - 3rd parameter (10000ms = 10s)
    if (client.pollConnection(1, 10000) < 0)
      delete[] buff;

    // TODO: Should this be a method in the client class?
    if ((client.getRevents() & POLLIN) == POLLIN) {
      ssize_t bytes_received = recv(client.getClientFd(), buff, BUFF_SIZE, 0);

      if (bytes_received < 0) {
        delete[] buff;
        std::cerr << RED << "Error: Receiving message" << END << std::endl;
        return (EXIT_FAILURE);
      }

      if (bytes_received > 0)
        std::cout << buff << std::endl;
    }

    if (client.pollConnection(1, 10000) < 0)
      delete[] buff;

    // TODO: Should this be a method in the client class?
    if ((client.getRevents() & POLLOUT) == POLLOUT) {
      if (send(client.getClientFd(), msg.c_str(), msg.length(), 0) < 0) {
        delete[] buff;
        std::cerr << "Error: Send failed" << std::endl;
        return (EXIT_FAILURE);
      }
    }
  }

  delete[] buff;
}
