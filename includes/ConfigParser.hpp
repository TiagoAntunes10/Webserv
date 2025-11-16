/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rvarela <rvarela@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/31 14:43:48 by rvarela           #+#    #+#             */
/*   Updated: 2025/11/14 15:37:33 by rvarela          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Webserver.hpp"
#include "ServerConfig.hpp"


class	ConfigParser {
	public:
		ConfigParser();
		ConfigParser(const ConfigParser& src);
		ConfigParser(const std::string& infile);
		~ConfigParser();

		ConfigParser	&operator=(const ConfigParser& src);

		
		std::vector<std::string>	getServers(std::string lines);
		std::vector<Server> 		returnServers();
		
		
		
		private:
		std::string					_infile;
		std::vector<ServerConfig>	_servers;
		
		
		//Methods
		void						readFile();
		std::vector<std::string>	tokenize(const std::string& str);
		void						FillServerBlocks(std::vector<std::string>& blocks);
		
		
		//aux
		static void					trimOuterSpaces(std::string& str);
		void						rmComments(std::string& str);
		size_t						findEndServer(std::string lines, size_t i);

	
}






//ERROR MANAGEMENT
class MsgError : public std::exception {
   
   public:
	MsgError(const std::string &str) throw();
	virtual const char *what() const throw();
	virtual ~MsgError() throw();

	
	private:
		std::string _msg;

};



static bool			hasQuotes(std::string str);
static std::string	toLower(const std::string& str);