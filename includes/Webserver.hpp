/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserver.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rvarela <rvarela@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/10 15:14:39 by rvarela           #+#    #+#             */
/*   Updated: 2025/10/24 18:37:10 by rvarela          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#define END "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"

// TODO: Remove these macros if they are not necessary
#define PORT 8080
#define BACKLOG 10
#define BUFF_SIZE 5000

// INCLUDES
#include "./Client.hpp"
#include "./HttpParser.hpp"
#include "./Socket.hpp"
#include "./utils.hpp"
#include <algorithm>
#include <cctype> //tolower
#include <cerrno>
#include <cstdlib> //exit() and EXIT_FAILURE
#include <cstring>
#include <iomanip>
#include <iostream>
#include <limits>
#include <poll.h>
#include <sstream> //VAR istringstream
#include <string>
#include <unistd.h>

// STL
#include <map>
#include <vector>

// CUSTOM VARS

#define MAX_CONECTIONS 100
#define URL_MAX_SIZE 8094 // Apache → 8.192 bytes    Nginx → 8.192 bytes

// SERVER CODE ERRORS
enum errServerCode {
  CONTINUE = 100,
  SWITCHING_PROTOCOLS = 101,

  OK = 200,
  CREATED = 201,
  ACCEPTED = 202,
  NON_AUTHORITATIVE_INFORMATION = 203,
  NO_CONTENT = 204,
  RESET_CONTENT = 205,
  PARTIAL_CONTENT = 206,

  MULTIPLE_CHOICES = 300,
  MOVED_PERMANENTLY = 301,
  FOUND = 302,
  SEE_OTHER = 303,
  NOT_MODIFIED = 304,
  USE_PROXY = 305,
  TEMPORARY_REDIRECT = 307,
  PERMANENT_REDIRECT = 308,

  BAD_REQUEST = 400,
  UNAUTHORIZED = 401,
  PAYMENT_REQUIRED = 402,
  FORBIDDEN = 403,
  NOT_FOUND = 404,
  METHOD_NOT_ALLOWED = 405,
  NOT_ACCEPTABLE = 406,
  PROXY_AUTHENTICATION_REQUIRED = 407,
  REQUEST_TIMEOUT = 408,
  CONFLICT = 409,
  GONE = 410,
  LENGTH_REQUIRED = 411,
  PRECONDITION_FAILED = 412,
  PAYLOAD_TOO_LARGE = 413,
  URI_TOO_LONG = 414,
  UNSUPPORTED_MEDIA_TYPE = 415,
  RANGE_NOT_SATISFIABLE = 416,
  EXPECTATION_FAILED = 417,
  MISDIRECTED_REQUEST = 421,
  UNPROCESSABLE_ENTITY = 422,
  UPGRADE_REQUIRED = 426,

  INTERNAL_SERVER_ERROR = 500,
  NOT_IMPLEMENTED = 501,
  BAD_GATEWAY = 502,
  SERVICE_UNAVAILABLE = 503,
  GATEWAY_TIMEOUT = 504,
  HTTP_VERSION_NOT_SUPPORTED = 505
};
