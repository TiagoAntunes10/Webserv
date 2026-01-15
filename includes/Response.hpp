#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "HttpParser.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

class Response {
public:
  Response(void);
  Response(Response const &resp);
  Response &operator=(Response const &resp);
  ~Response(void);

  std::string buildResponse(int status_code, http_Data data_req);
  void buildStatusLine(int status_code, http_Data data_req);
  void buildHeaders(http_Data data_req);
  bool buildBody(http_Data data_req);

private:
  void formatHeader(std::string key, std::string value, bool end);
  void defaultHeaders(void);
  std::string status_line_;
  std::string headers_;
  std::string body_;
};

#endif
