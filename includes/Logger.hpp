#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <cstdarg>
#include <sstream>

class Logger {
public:
  ~Logger(void);

  // NOTE: There isn't a need for this right now, but it my be useful
  // Logger &getLogger(void);

  static void consoleMsg(std::ostream &io, std::string const colour,
                         std::string format, ...);

private:
  typedef std::string::iterator iterator;

  Logger(void) {};
  Logger(Logger const &log) { (void)log; };
  Logger &operator=(Logger const &log) {
    (void)log;
    return (*this);
  };
  static Logger log_;
};

#define END "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"

#endif
