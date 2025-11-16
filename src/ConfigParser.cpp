/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rvarela <rvarela@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/31 14:48:37 by rvarela           #+#    #+#             */
/*   Updated: 2025/11/14 15:36:56 by rvarela          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigParser.hpp"


//Class for parsisng Configuration files --------> ConfigParser
ConfigParser::ConfigParser() {}

ConfigParser::ConfigParser(const ConfigParser& src) {
	*this = src;
}

ConfigParser::ConfigParser(const std::string& infile) : _infile(infile) {
	
}

ConfigParser::~ConfigParser() {}


//Overload operators definition
ConfigParser	&ConfigParser::operator=(const ConfigParser& src) {
	if (this != &src) {
		this->_infile = src.infile;
		//adicionar mais variáveis ?
	}
	
	return (*this);
}


/* Parse a loaded configuration file
* send to a vector of servers
* throw a std::runtime_error if something goes wrong (can not open file or empty file)
*/

void	ConfigParser::readFile() {
	std::ifstream	file(_infile.c_str());

	if (!file.isopen())
		throw MsgError("Unable to read from " + _infile);

		
	//Read config file into string lines
	std::string	lines;
	lines.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
	file.close();

	if (!lines.empty()) {
		trimOuterSpaces(lines);
		rmComments(lines);
	}
		
	if (lines.empty())
		throw MsgError("Empty configuration file!");
	
	if (hasQuotes(lines))
		throw MsgError("Quotes not supported in configuration file!");
	
	
	std::vector<std::string> serverBlocks = getServers(lines);
	FillServerBlocks(serverBlocks);
	
}



void	ConfigParser::FillServerBlocks(std::vector<std::string>& blocks) {


	//TODO

	

	
	
}




std::vector<Server> ConfigParser::returnServers() { 
	
	return (this->_servers); 

}



std::vector<std::string>	ConfigParser::tokenize(const std::string& str) {
	
	std::istringstream			iss(str);
	std::vector<std::string>	tokens;
	std::string					token;
	
	while (iss >> token) {
		ConfigParser::trimOuterSpaces(token);
		tokens.push_back(token);
	}
	
	return (tokens);
}


size_t	ConfigParser::findEndServer(std::string lines, size_t i) {
	size_t	j = 0;
	bool opened = false;
	
	while (lines[i]) {
		if (lines[i] == '{') {
			j++;
			opened = true;
		}
		else if (lines[i] == '}') {
			j--;
			if (j == 0 && opened)
				return (i);
			else if (j < 0)
				throw MsgError("} not well defined!");
		}
		i++;
	}

	throw MsgError("Opened server block!");
	
}



std::vector<std::string>	ConfigParser::getServers(std::string lines) {
	
	std::vector<std::string>	servers;
	size_t			begin = 0;
	std::string		SField;

	while (content[start]) {
		SField = (lines.substr(begin, 6));
		if (toLower(SField) != "server")
			throw MsgError("No server defined!");
		
		begin += 6;
		while (std::isspace(lines[begin]))
			begin++;
		
		if (lines[begin] != '{')
			throw MsgError("Not correct server definition!");

		size_t end = findEndServer(lines, begin); //till find }

		while (isspace(lines[++begin])) {}
		
		size_t endBlock = end;
		while (std::isspace(lines[--endBlock])) {}

		servers.push_back(lines.substr(begin, endBlock - begin + 1));
		begin = end + 1;

		while (lines[begin] && std::isspace(lines[begin]))
			begin++;
		
	}

	return (servers);
	
}





//AUX FUNCTIONS

void	ConfigParser::trimOuterSpaces(std::string &str) {
	/*if (str.empty()) 
		return;*/
	while (std::isspace(str[0]) && !str.empty())
		str.erase(str.begin());
	if (str.empty())
		return;
	while (std::isspace(str[str.length() - 1]) && !str.empty())
		str.erase(str.end() - 1);
}


void	ConfigParser::rmComments(std::string& str) {
	size_t hash = str.find('#');
	
	while (hash != std::string::npos) {
		size_t endl = str.find('\n', hash);
		str.erase(hash, endl - hash);
		hash = str.find('#');
	}
}


//AUX FUNCTIONS OUT OF CLASS

// Checks if a string has double or single quotes
static bool	hasQuotes(std::string str) {
	size_t dQuote = str.find("\"");
	size_t sQuote = str.find("\'");
	
	return (dQuote != std::string::npos || sQuote != std::string::npos);
}


//tolower chars in a string
static std::string	toLower(const std::string& str) {
    std::string lower_str = str;
	for (std::string::iterator it = lower_str.begin(); it != lower_str.end();
		 ++it) {
		*it = std::tolower(static_cast<unsigned char>(*it));
	}
	return lower_str;
}




//ERROR MANAGEMENT

ConfigError::MsgError(const std::string &str) throw() {
	_msg = "ERROR: " + str + '\n';
}

const char *MsgError::what() const throw() { return (_msg.c_str()); }

MsgError::~MsgError() throw() {}

