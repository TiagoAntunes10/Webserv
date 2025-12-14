#include "../includes/Webserver.hpp"

int set_non_blocking(int fd) {
  int opt;

  opt = fcntl(fd, F_GETFL);
  if (opt < 0) {
    Logger::getLogger().consoleMsg(stderr, RED,
                                   "Error: getting socket status flags");
    return (0);
  }

  opt = opt | O_NONBLOCK;
  if (fcntl(fd, F_SETFL, opt) < 0) {
    Logger::getLogger().consoleMsg(stderr, RED,
                                   "Error: getting socket status flags");
    return (0);
  }
  return (1);
}
