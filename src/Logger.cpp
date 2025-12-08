#include "../includes/Webserver.hpp"

Logger::~Logger(void) {}

// Logger &Logger::getLogger(void) { return (log_); }

void Logger::consoleMsg(std::ostream &io, std::string const colour,
                        std::string format, ...) {
  std::stringstream stream;
  va_list args;
  iterator begin = format.begin();
  iterator end = format.end();

  va_start(args, format);

  for (; begin != end; begin++) {
    if (*begin == '%') {
      continue;
    } else {
      switch (*begin) {
      case 's': {
        char *str = va_arg(args, char *);
        stream << str;
        break;
      }

      case 'd': {
        unsigned int num = va_arg(args, unsigned int);
        stream << num;
        break;
      }

      default:
        Logger::consoleMsg(std::cerr, RED, "%s",
                           "Error: not a valid format string for logging");
      }
    }
  }

  va_end(args);

  io << colour << stream.str() << END << std::endl;
}
