/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rvarela <rvarela@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/03 16:18:50 by rvarela           #+#    #+#             */
/*   Updated: 2025/10/03 16:25:48 by rvarela          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserver.hpp"
#include "Config.hpp"

int	main(int ac, char** av) {
	
	if (ac > 2) {
		std::cout << "USAGE: ./webserv [config file]" << std::endl;
		return (1);
	}

	ConfigParser	confParser("conf/default.conf");
	
	if (av[1])
		confParser = ConfigParser(av[1]);



	return (0);
}