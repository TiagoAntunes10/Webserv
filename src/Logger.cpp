#include "../includes/Webserver.hpp"

Logger::~Logger(void) {}

void Logger::initLogger(void) { init_time_ = time(NULL); }

Logger &Logger::getLogger(void) {
  static Logger logger;

  return (logger);
}

// TODO: Maybe change the time stamps to microseconds
void Logger::consoleMsg(FILE *io, const char *colour, const char *format, ...) {
  va_list args;

  fprintf(io, "[%ld]		", (time(NULL) - init_time_));

  va_start(args, format);

  if (colour) {
    fprintf(io, "%s", colour);
    vfprintf(io, format, args);
    fprintf(io, "%s", END);
  } else
    vfprintf(io, format, args);

  fprintf(io, "\n");

  va_end(args);
}
