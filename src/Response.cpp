#include "../includes/Webserver.hpp"
#include <iterator>

Response::Response(void) {}

Response::Response(Response const &resp) {
  status_line_ = resp.status_line_;
  headers_ = resp.headers_;
  body_ = resp.body_;
}

Response &Response::operator=(Response const &resp) {
  status_line_ = resp.status_line_;
  headers_ = resp.headers_;
  body_ = resp.body_;

  return (*this);
}

Response::~Response(void) { return; }

std::string Response::buildResponse(int status_code, http_Data data_req) {
  if (!buildBody(data_req))
    status_code = NOT_FOUND;

  buildStatusLine(status_code, data_req);
  buildHeaders(data_req);

  std::string response = status_line_ + headers_ + body_;

  std::cout << MAGENTA << response << END << std::endl;

  return (response);
}

void Response::buildStatusLine(int status_code, http_Data data_req) {
  status_line_ = data_req.version + " ";
  std::stringstream status;

  status << status_code;
  switch (status_code) {
  case OK:
    status_line_ += status.str() + " OK\r\n";
    break;
  case BAD_REQUEST:
    status_line_ += status.str() + " Bad Request\r\n";
    break;
  case NOT_FOUND:
    status_line_ += status.str() + " Not Found\r\n";
    break;
  case METHOD_NOT_ALLOWED:
    status_line_ += status.str() + " Method Not Allowed\r\n";
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

void Response::buildHeaders(http_Data data_req) {
  // TODO: Check if the data_req information is needed
  (void)data_req;
  defaultHeaders();
}

bool Response::buildBody(http_Data data_req) {
  if (!data_req.uri.compare("/"))
    return (true);

  std::string clean_uri = data_req.uri.substr(1, data_req.uri.length() - 1);

  try {
    std::string line;
    std::ifstream file;

    file.open(clean_uri.c_str());

    while (std::getline(file, line))
      body_ += line + "\n";

    if (file.eof())
      body_ = body_.substr(0, body_.length() - 1);

    file.close();

    body_ += "\r\n";

    return (true);
  } catch (...) {
    return (false);
  }
}

void Response::formatHeader(std::string key, std::string value, bool end) {
  headers_ += key + ":" + value + "\r\n";

  if (end)
    headers_ += "\r\n";
}

void Response::defaultHeaders(void) {
  std::stringstream length;

  length << body_.size();

  formatHeader("Content-Length", length.str(), false);
  formatHeader("Connection", "close", false);
  // NOTE: "text/plain" is another possible option
  formatHeader("Content-Type", "text/html", true);
}
