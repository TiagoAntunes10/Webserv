/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rvarela <rvarela@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/07 16:40:03 by rvarela           #+#    #+#             */
/*   Updated: 2025/11/07 16:40:47 by rvarela          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <vector>
#include <map>

struct LocationConfig {
    std::string path;
    std::string root;
    std::vector<std::string> methods;
    bool autoindex;
    std::map<int, std::string> error_pages;
    std::string cgi_path;
    std::vector<std::string> cgi_extensions;
    std::string return_url;
};

struct ServerConfig {
    int listen;
    std::string server_name;
    std::string root;
    std::vector<std::string> index;
    std::map<int, std::string> error_pages;
    size_t client_max_body_size;
    std::vector<LocationConfig> locations;
};

