#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "HttpParser.hpp"
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
// TODO: Confirm if it is ok to use this library
#include <unistd.h>

class Response {
public:
  typedef std::multimap<std::string, std::string>::const_iterator
      multimap_iterator;

  Response(void);
  Response(Response const &resp);
  Response &operator=(Response const &resp);
  ~Response(void);

  std::string buildResponse(int status_code, http_Data data_req);
  void buildStatusLine(http_Data const &data_req);
  // TODO: Ensure if http_Data is necessary as an argument
  void buildHeaders(http_Data);
  bool buildBody(http_Data const &data_req);

private:
  void formatHeader(std::string const &key, std::string const &value, bool end);
  void defaultHeaders(void);
  bool checkRequest(http_Data const &data_req);
  bool readBody(void);
  std::string findFile(std::string const &path, http_Data const &data_req);
  bool postInfo(std::string const &path, http_Data const &data_req);
  bool deleteInfo(std::string const &path);

  std::string status_line_;
  std::string headers_;
  std::string body_;
  std::string complete_uri_;
  HttpMethod method_;
  int response_code_;
};

#endif
