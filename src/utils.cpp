#include "../includes/Webserver.hpp"

void set_non_blocking(int fd) {
  int opt;

  opt = fcntl(fd, F_GETFL);
  if (opt < 0) {
    std::cerr << RED << "Error: getting socket status flags" << END
              << std::endl;
    exit(EXIT_FAILURE);
  }

  opt = opt | O_NONBLOCK;
  if (fcntl(fd, F_SETFL, opt) < 0) {
    std::cerr << RED << "Error: setting socket status flags" << END
              << std::endl;
    exit(EXIT_FAILURE);
  }
}
