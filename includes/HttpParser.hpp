/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpParser.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rvarela <rvarela@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/10 15:10:33 by rvarela           #+#    #+#             */
/*   Updated: 2025/10/31 14:45:20 by rvarela          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <map>
#include <sstream> //Atualizar o nosso header base (o nome que for)

/*
 *Guardar variáveis de data numa struct é mais limpo e funciona melhor
 *class é para a lógica do parser e a struct para a data
 *a class estática de utilidade preenche a estrutura de dados que pode ser
 * acedida
 */

enum HttpMethod { GET = 1, POST, DELETE, PUT, UNKNOWN };

// REQUEST DATA (guarda informação do pedido HTTP)
struct http_Data {

  enum HttpMethod method;

  std::string uri;
  std::string decodedUri;
  std::string version;
  std::string body;

  std::multimap<std::string, std::string> header;
  std::map<std::string, std::string> queryParams;

  http_Data() : method(UNKNOWN) {}
};

// Overload operator
std::ostream &operator<<(std::ostream &out, const http_Data &req);

// REQUEST PARSER (verifica e preenche a informação do pedido HTTP)
// Class for parsisng HTTP requests
class HttpParser {
public:
  static unsigned short parseHeader(const std::string &request,
                                    http_Data &httpReq);

  // HttpParser();  // passar para privado no programa geral
  // ~HttpParser(); // passar para privado no programa geral

private:
  // Class is not instantiable
  HttpParser();
  ~HttpParser();

  // Methods
  static bool existMethod(const std::string &method);
  static bool isValidMethod(const std::string &method);
  static bool isValidVersion(const std::string &version);
  static bool isValidUrl(const std::string &url);
  static std::string decode(const std::string &encoded);

  static std::string trimWhiteBounds(const std::string &str);
  static void trimNullEnd(std::string &str);
  static std::string toLower(const std::string &str);
};
