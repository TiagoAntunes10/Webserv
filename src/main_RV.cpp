/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rvarela <rvarela@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/03 16:18:50 by rvarela           #+#    #+#             */
/*   Updated: 2025/11/14 15:36:06 by rvarela          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserver.hpp"
#include "HttpParser.hpp"
#include "ConfigParser.hpp"


int	main(int ac, char** av) {
	
	if (ac > 2) {
		std::cout << "USAGE: ./webserv [config file]" << std::endl;
		return (1);
	}

	ConfigParser	confParser("conf/default.conf");
	
	if (av[1])
		confParser = ConfigParser(av[1]);
	

    std::vector<Server>     servers;  // Defined outside the try block so it can be used later

	try {
		confParser.readFile();
		servers = confParser.returnServers();
	} catch (std::exception& e) {
		std::cerr << e.what();
		return 1;
	}




    
    //HTTP REQUEST parser test________________________________________________   

    std::string request =
        "POST /upload?file=test.txt HTTP/1.1\r\n"
        "Host: localhost:8080\r\n"
        "User-Agent: curl/7.68.0\r\n"
        "Accept: text/html, application/xhtml+xml, application/xml;q=0.9, */*;q=0.8\r\n"
        "Transfer-Encoding: chunked\r\n"
        "Content-Type: text/plain\r\n"
        "Connection: keep-alive\r\n"
        "\r\n"
        "7\r\n"
        "Hello, \r\n"
        "6\r\n"
        "world!\r\n"
        "0\r\n"
        "\r\n";

    http_Data dataReq;
    HttpParser parser;

    int responseSTAT = (parser.parseHeader(request, dataReq));
    
    if (responseSTAT > 0)
        std::cout << dataReq << std::endl;
    else {
        std::cerr << "❌ Invalid HTTP request" << std::endl;
        return (-1);
    }




		

	return (responseSTAT);
}