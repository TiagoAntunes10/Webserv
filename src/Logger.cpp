#include "../includes/Webserver.hpp"

Logger::~Logger(void) {}

// Logger &Logger::getLogger(void) { return (log_); }

void Logger::consoleMsg(std::ostream &io, std::string const colour,
                        std::string const msg) {
  io << colour << msg << END << std::endl;
}
