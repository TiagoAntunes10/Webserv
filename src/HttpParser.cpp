/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rvarela <rvarela@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/10 15:20:14 by rvarela           #+#    #+#             */
/*   Updated: 2025/10/31 14:58:57 by rvarela          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Webserver.hpp"

// HttpParser class to parse HTTP requests: request line / headers / body
// Validates the request format and extracts info to HTTP REQUEST object
// Checks Methods (GET, POST, DELETE) and checks bad requests and update the
// response status

// Request status
static int responseSTAT = OK;

HttpParser::HttpParser() {}
HttpParser::~HttpParser() {}

// Class for parsisng HTTP requests --------> HttpParser
unsigned short HttpParser::parseHeader(const std::string &request,
                                       http_Data &httpReq) {
  responseSTAT = OK;

  // Check if request is valid
  if (request.empty() ||
      (request.find_first_not_of("\t\r\n") == std::string::npos)) {
    responseSTAT = BAD_REQUEST;
    return (responseSTAT);
  }

  // Get request and HEADER parse
  std::istringstream iss(request);
  std::string line;

  if (!std::getline(iss, line)) {
    responseSTAT = -1;
    return (responseSTAT);
  }

  std::istringstream firstline(line);
  std::string methodStr;
  firstline >> methodStr >> httpReq.uri >> httpReq.version;

  if (methodStr == "GET")
    httpReq.method = GET;
  else if (methodStr == "POST")
    httpReq.method = POST;
  else if (methodStr == "DELETE")
    httpReq.method = DELETE;
  else if (methodStr == "PUT")
    httpReq.method = PUT;
  else
    httpReq.method = UNKNOWN;

  if (!isValidMethod(methodStr) || methodStr.size() == 0) {
    responseSTAT = METHOD_NOT_ALLOWED;
    return (responseSTAT);
  }

  if (existMethod(methodStr)) {
    responseSTAT = METHOD_NOT_ALLOWED;
    return (responseSTAT);
  }

  if (httpReq.version.size() == 0 || !isValidVersion(httpReq.version)) {
    responseSTAT = HTTP_VERSION_NOT_SUPPORTED;
    return (responseSTAT);
  }

  httpReq.decodedUri = decode(httpReq.uri);
  if (httpReq.decodedUri.size() == 0 || !isValidUrl(httpReq.decodedUri)) {
    responseSTAT = BAD_REQUEST;
    return (responseSTAT);
  }

  if (httpReq.decodedUri.size() > URL_MAX_SIZE) {
    responseSTAT = URI_TOO_LONG;
    return (responseSTAT);
  }

  // Headers
  while (std::getline(iss, line) && line != "\r" && !line.empty()) {
    size_t Cpos = line.find(":");
    if (Cpos != std::string::npos) {
      std::string key = trimWhiteBounds(line.substr(0, Cpos));
      key = toLower(key);
      std::string values = trimWhiteBounds(line.substr(Cpos + 1));
      if (key.empty() || values.empty()) {
        responseSTAT = BAD_REQUEST;
        return (responseSTAT);
      }

      std::stringstream ss(values);
      std::string value;

      // TODO: Should we separate the values by the commas? Maybe leave them
      // together and create a function to separate the values when necessary
      // NOTE: User-agent gives problems if the values are separated by their
      // commmas. Are there cases where it is necessary?
      if (key == "date" || key == "if-modified-since" ||
          key == "last-modified" || key == "user-agent") {
        httpReq.header.insert(std::make_pair(key, values));
      } else {
        while (std::getline(ss, value, ',')) {
          size_t start = value.find_first_not_of(" \n\r\t");
          size_t end = value.find_last_not_of(" \n\r\t");

          if (start != std::string::npos)
            value = value.substr(start, end - start + 1);
          else
            value.clear();

          if (!value.empty()) // ignora entradas vazias
            httpReq.header.insert(std::make_pair(key, value));
        }
      }
    } else {
      responseSTAT = BAD_REQUEST;
      return responseSTAT;
    }
  }

  if (httpReq.header.count("user-agent") != 1) {
    responseSTAT = BAD_REQUEST;
    // return responseSTAT;
  }

  // Get body
  std::string body((std::istreambuf_iterator<char>(iss)),
                   std::istreambuf_iterator<char>());
  // Append body
  httpReq.body = body;

  // Query parameters
  std::size_t IPos = httpReq.uri.find('?');
  if (IPos != std::string::npos) {

    std::string query = httpReq.uri.substr(IPos + 1); // Find query
    httpReq.uri = httpReq.uri.substr(0, IPos);        // Extract query
    std::stringstream ssq(query);
    std::string Qpair;

    while (std::getline(ssq, Qpair, '&')) {
      size_t equalPos = Qpair.find('=');
      std::string keyq;
      std::string valueq;
      if (equalPos == std::string::npos)
        keyq = Qpair; // empty
      else {
        keyq = Qpair.substr(0, equalPos);
        valueq = Qpair.substr(equalPos + 1);
      }
      trimWhiteBounds(keyq);
      trimWhiteBounds(valueq);

      if (!keyq.empty())
        httpReq.queryParams.insert(
            std::pair<std::string, std::string>(keyq, valueq));
    }
  }

  return (responseSTAT);
}

// AUX BOOL FUNCTIONS

bool HttpParser::existMethod(const std::string &method) {
  if (method == "PUT" || method == "HEAD" || method == "OPTIONS" ||
      method == "PATCH")
    return true;
  return false;
}

bool HttpParser::isValidMethod(const std::string &method) {
  if (method == "GET" || method == "POST" || method == "DELETE")
    return true;
  return false;
}

bool HttpParser::isValidVersion(const std::string &version) {
  if (version == "HTTP/1.1" || version == "HTTP/1.0" || version == "HTTP/0.9")
    return true;
  return false;
}

/* URL must start with a '/', not contain fragment identifiers (indicated by
'#'), contains at most one '?', and has a valid number of '=' and '&' characters
when a '?' is present (must match the number of '&' characters plus one). */
bool HttpParser::isValidUrl(const std::string &url) {
  if (url[0] != '/')
    return false;
  if (url.find("#") != std::string::npos)
    return false;
  if (std::count(url.begin(), url.end(), '?') > 1)
    return false;
  if ((url.find('?') != std::string::npos) &&
      (std::count(url.begin(), url.end(), '=') !=
       std::count(url.begin(), url.end(), '&') + 1))
    return false;

  return true;
}

std::string HttpParser::decode(const std::string &encoded) {
  std::ostringstream decoded;
  for (size_t i = 0; i < encoded.length(); ++i) {
    if (encoded[i] == '%' && i + 2 < encoded.length() &&
        std::isxdigit(encoded[i + 1]) && std::isxdigit(encoded[i + 2])) {
      int value;
      std::istringstream ss(encoded.substr(i + 1, 2));
      ss >> std::hex >> value;
      decoded << static_cast<char>(value);
      i += 2;
    } else {
      decoded << encoded[i];
    }
  }
  return decoded.str();
}

// AUX FUNCTIONS

// trim whitespaces ( space \t \r \n) from begin and ened of a string. Return a
// trimmed result
std::string HttpParser::trimWhiteBounds(const std::string &str) {
  size_t start = str.find_first_not_of(" \t\r\n");
  if (start == std::string::npos)
    return "";

  size_t end = str.find_last_not_of(" \t\r\n");

  return (str.substr(start, (end - start + 1)));
}

// trim null at the end of a string. Save the string trimmed of null chars
void HttpParser::trimNullEnd(std::string &str) {
  size_t end = str.find_last_not_of("\0");

  if (end == std::string::npos)
    str.clear();
  else
    str.resize(end + 1);
}

// tolower chars in a string
std::string HttpParser::toLower(const std::string &str) {
  std::string lower_str = str;
  for (std::string::iterator it = lower_str.begin(); it != lower_str.end();
       ++it) {
    *it = std::tolower(static_cast<unsigned char>(*it));
  }
  return lower_str;
}

// Overload operators definition
std::ostream &operator<<(std::ostream &out, const http_Data &req) {
  out << "--------------HTTP REQUEST---------------" << std::endl;
  out << "Method: ";
  switch (req.method) {
  case GET:
    out << "GET";
    break;
  case POST:
    out << "POST";
    break;
  case DELETE:
    out << "DELETE";
    break;
  case PUT:
    out << "PUT";
    break;
  default:
    out << "UNKNOWN";
    break;
  }
  out << std::endl;
  out << "URI: " << req.uri << std::endl;
  out << "Protocol version: " << req.version << std::endl;
  out << "Header fields: " << std::endl;
  for (std::multimap<std::string, std::string>::const_iterator it =
           req.header.begin();
       it != req.header.end(); it++)
    out << "\t" << it->first << ": " << it->second << std::endl;

  out << "Query parameters: " << std::endl;
  for (std::multimap<std::string, std::string>::const_iterator it =
           req.queryParams.begin();
       it != req.queryParams.end(); it++)
    out << "\t" << it->first << ": " << it->second << std::endl;

  out << "Body message: " << req.body << std::endl;

  return (out);
}
