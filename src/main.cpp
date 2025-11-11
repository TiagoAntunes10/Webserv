#include "../includes/include.hpp"

int main(void) {
  int server_fd;
  struct sockaddr_in server_addr;

  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    return (EXIT_FAILURE);

  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(PORT);

  if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) <
      0) {
    close(server_fd);
    return (EXIT_FAILURE);
  }

  if (listen(server_fd, BACKLOG) < 0) {
    close(server_fd);
    return (EXIT_FAILURE);
  }

  std::cout << RED << "Listening on port " << PORT << "..." << END << std::endl;

  struct sockaddr_in client_addr;
  socklen_t client_addr_len = sizeof(client_addr);
  int client_fd;

  char *buff = new char(BUFF_SIZE);
  struct pollfd poll_fd;
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

  while ((poll_fd.revents & POLLHUP) != POLLHUP) {
    if ((client_fd = accept(server_fd, (struct sockaddr *)&client_addr,
                            &client_addr_len)) < 0) {
      close(server_fd);
      return (EXIT_FAILURE);
    }

    poll_fd.fd = client_fd;
    poll_fd.events = POLLIN | POLLOUT;
    // NOTE: Timeout is in miliseconds - 3rd parameter (10000ms = 10s)
    if (poll(&poll_fd, 1, 10000) < 0) {
      close(client_fd);
      close(server_fd);
      delete[] buff;
      std::cerr << "Error in Poll" << std::endl;
      return (EXIT_FAILURE);
    }

    if ((poll_fd.revents & POLLIN) == POLLIN) {
      ssize_t bytes_received = recv(client_fd, buff, BUFF_SIZE, 0);

      if (bytes_received < 0) {
        close(client_fd);
        close(server_fd);
        delete[] buff;
        std::cerr << "Error in recv" << std::endl;
        return (EXIT_FAILURE);
      }

      std::cout << BLUE << bytes_received << END << std::endl;

      if (bytes_received > 0)
        std::cout << buff << std::endl;
    }

    if (poll(&poll_fd, 1, 10000) < 0) {
      close(client_fd);
      close(server_fd);
      delete[] buff;
      std::cerr << "Error in Poll" << std::endl;
      return (EXIT_FAILURE);
    }

    if ((poll_fd.revents & POLLOUT) == POLLOUT) {
      if (send(client_fd, msg.c_str(), msg.length(), 0) < 0) {
        close(client_fd);
        close(server_fd);
        delete[] buff;
        std::cerr << "Error: Send failed." << std::endl;
        return (EXIT_FAILURE);
      }
    }

    close(client_fd);
  }

  close(server_fd);
  delete[] buff;
}
