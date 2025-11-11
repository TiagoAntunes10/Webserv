#define END "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"

#include <arpa/inet.h>
#include <cerrno>
#include <cstdlib>
#include <iostream>
#include <poll.h>
#include <sstream>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080
#define BACKLOG 10
#define BUFF_SIZE 4096
