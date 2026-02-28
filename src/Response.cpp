#include "../includes/Webserver.hpp"
#include <fcntl.h>

Response::Response(void) {}

Response::Response(Response const &resp) {
  status_line_ = resp.status_line_;
  headers_ = resp.headers_;
  body_ = resp.body_;
  complete_uri_ = resp.complete_uri_;
  method_ = resp.method_;
  response_code_ = resp.response_code_;
}

Response &Response::operator=(Response const &resp) {
  status_line_ = resp.status_line_;
  headers_ = resp.headers_;
  body_ = resp.body_;
  complete_uri_ = resp.complete_uri_;
  method_ = resp.method_;
  response_code_ = resp.response_code_;

  return (*this);
}

Response::~Response(void) { return; }

// NOTE: Allowed methods GET, POST, DELETE, PUT and HEAD
std::string Response::buildResponse(int status_code, http_Data data_req) {
  method_ = data_req.method;
  response_code_ = status_code;

  // NOTE: Checking the allowed methods can be made before and the methods not
  // allowed go to category "UNKNOWN"
  if (response_code_ == OK && method_ == UNKNOWN)
    response_code_ = METHOD_NOT_ALLOWED;

  // TODO: It might be better to set the NOT_FOUND code in the functions
  buildBody(data_req);

  if (response_code_ == OK && body_.size() > BODY_MAX_SIZE)
    response_code_ = PAYLOAD_TOO_LARGE;

  buildStatusLine(data_req);
  buildHeaders(data_req);

  std::string response = status_line_ + headers_;
  if (method_ != HEAD)
    response += body_;

  std::cout << MAGENTA << response << END << std::endl;

  return (response);
}

void Response::buildStatusLine(http_Data const &data_req) {
  status_line_ = data_req.version + " ";
  std::stringstream status;

  status << response_code_;
  switch (response_code_) {
  case OK:
    status_line_ += status.str() + " OK\r\n";
    break;
  case CREATED:
    status_line_ += status.str() + " Created\r\n";
    break;
  case NO_CONTENT:
    status_line_ += status.str() + " No Content\r\n";
    break;
  case BAD_REQUEST:
    status_line_ += status.str() + " Bad Request\r\n";
    break;
  case NOT_FOUND:
    status_line_ += status.str() + " Not Found\r\n";
    break;
  case NOT_ACCEPTABLE:
    status_line_ += status.str() + " Not Acceptable\r\n";
    break;
  case METHOD_NOT_ALLOWED:
    status_line_ += status.str() + " Method Not Allowed\r\n";
    break;
  case PAYLOAD_TOO_LARGE:
    status_line_ += status.str() + " Payload Too Large\r\n";
    break;
  case HTTP_VERSION_NOT_SUPPORTED:
    status_line_ += status.str() + " HTTP Version Not Supported\r\n";
    break;
  case URI_TOO_LONG:
    status_line_ += status.str() + " URI Too Long\r\n";
    break;
  case INTERNAL_SERVER_ERROR:
    status_line_ += status.str() + " Internal Server Error\r\n";
    break;
  default:
    Logger::getLogger().consoleMsg(stderr, RED,
                                   "Error: Status code not supported");
    return;
  }
}

void Response::buildHeaders(http_Data) { defaultHeaders(); }

// TODO: The path will need to use information from the config file
bool Response::buildBody(http_Data const &data_req) {
  if (!data_req.uri.compare("/"))
    return (true);

  // TODO: Find how to save the NOT_ACCEPTABLE status code. Not sure what is the
  // best way to do it (maybe have a private property to save the status code)
  complete_uri_ =
      findFile(data_req.uri.substr(1, data_req.uri.length() - 1), data_req);

  if ((method_ == GET || method_ == HEAD || method_ == DELETE) &&
      complete_uri_.empty())
    return (false);

  if (method_ == GET || method_ == HEAD) {
    readBody();
  } else if (method_ == POST || method_ == PUT) {
    // TODO: Add Multiform (content-type)
    return (
        postInfo(data_req.uri.substr(1, data_req.uri.length() - 1), data_req));
  } else if (method_ == DELETE) {
    return (deleteInfo(complete_uri_));
  }

  return (true);
}

void Response::formatHeader(std::string const &key, std::string const &value,
                            bool end) {
  if (!value.empty())
    headers_ += key + ": " + value + "\r\n";

  if (end)
    headers_ += "\r\n";
}

void Response::defaultHeaders(void) {
  std::stringstream length;

  length << body_.size();

  size_t begin_pos = complete_uri_.find_last_of(".") + 1;
  std::string extension =
      complete_uri_.substr(begin_pos, complete_uri_.length() - begin_pos);

  // NOTE: Get the date when the file requested has been last modified
  std::string date_modified;
  struct stat *statbuf = 0;
  if (stat(complete_uri_.c_str(), statbuf) == 0) {
    struct tm *time_info = localtime(&statbuf->st_atim.tv_sec);
    char char_date[32];

    strftime(char_date, 32, "%a, %d %b %Y %H:%M:%S %Z", time_info);
    date_modified = char_date;
  }

  formatHeader("Content-Type", Mime::getMimeInfo().getMimeType(extension),
               false);
  formatHeader("Date", Logger::getLogger().getCurrentTime(), false);
  formatHeader("Last-Modified", date_modified, false);
  formatHeader("Content-Length", length.str(), false);
  formatHeader("Connection", "close", true);
}

// TODO: Is this needed? How will the program handle the methods when they are
// defined by the config file
bool Response::checkRequest(http_Data const &) { return (true); }

bool Response::readBody(void) {
  try {
    std::string line;
    std::ifstream file;

    file.open(complete_uri_.c_str());

    while (std::getline(file, line))
      body_ += line + "\n";

    if (file.eof())
      body_ = body_.substr(0, body_.length() - 1);

    file.close();

    body_ += "\r\n";

    return (true);
  } catch (...) {
    response_code_ = NOT_FOUND;

    return (false);
  }
}

std::string Response::findFile(std::string const &path,
                               http_Data const &data_req) {
  std::string dir_path = path.substr(0, path.find_last_of("/"));
  DIR *dir = opendir(dir_path.c_str());

  if (!dir) {
    return ("");
  }

  struct dirent *dir_entry = readdir(dir);
  std::string file_path;
  std::string curr_file;
  std::string key = "accept";
  std::pair<multimap_iterator, multimap_iterator> ret;

  ret = data_req.header.equal_range(key);

  // TODO: Don't really like how this is made, but it kind of works
  while (dir_entry) {
    curr_file = dir_path + "/" + dir_entry->d_name;

    for (multimap_iterator it = ret.first; it != ret.second; it++) {
      // NOTE: If the request accepts any kind of file, we find the first that
      // matches the path in the request
      if (it->second == "*/*") {
        if (path == curr_file) {
          file_path = curr_file;
          closedir(dir);

          return (file_path);
        }
      } else {
        if (path.find_last_of(".") == std::string::npos) {
          std::string file_path_type =
              path + Mime::getMimeInfo().getExtension(it->second);

          if (file_path_type == curr_file) {
            file_path = curr_file;
            closedir(dir);

            return (file_path);
          }
        } else {
          if (path == curr_file) {
            file_path = curr_file;
            closedir(dir);

            return (file_path);
          }
        }
      }
    }

    dir_entry = readdir(dir);
  }

  closedir(dir);

  response_code_ = NOT_FOUND;
  return (file_path);
}

bool Response::postInfo(std::string const &path, http_Data const &data_req) {
  int file_fd;
  std::string extension = Mime::getMimeInfo().getExtension(
      data_req.header.find("content-type")->second);
  std::string full_path = path + "." + extension;

  if (access(full_path.c_str(), F_OK) < 0) {
    file_fd = open(full_path.c_str(), O_CREAT | O_RDWR);
    response_code_ = CREATED;

    if (file_fd < 0) {
      response_code_ = INTERNAL_SERVER_ERROR;

      return (false);
    }
  } else {
    if (method_ == POST)
      file_fd = open(full_path.c_str(), O_WRONLY | O_APPEND);
    else if (method_ == PUT) {
      if (unlink(full_path.c_str()) < 0) {
        response_code_ = INTERNAL_SERVER_ERROR;

        return (false);
      }

      file_fd = open(full_path.c_str(), O_CREAT | O_RDWR);
    }

    if (file_fd < 0) {
      response_code_ = INTERNAL_SERVER_ERROR;

      return (false);
    }
  }

  // TODO: Add case with multiform type
  if (write(file_fd, data_req.body.c_str(), data_req.body.length()) < 0) {
    response_code_ = INTERNAL_SERVER_ERROR;
    close(file_fd);

    return (false);
  }

  close(file_fd);
  return (true);
}

bool Response::deleteInfo(std::string const &path) {
  if (access(path.c_str(), F_OK) < 0) {
    response_code_ = NOT_FOUND;
    return (false);
  }

  // TODO: Confirm if we can use this function (not a lot of other options)
  if (unlink(path.c_str()) < 0) {
    response_code_ = INTERNAL_SERVER_ERROR;
    return (false);
  }

  response_code_ = NO_CONTENT;
  return (true);
}
