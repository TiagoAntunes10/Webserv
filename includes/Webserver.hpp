/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserver.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rvarela <rvarela@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/10 15:14:39 by rvarela           #+#    #+#             */
/*   Updated: 2025/10/10 15:19:37 by rvarela          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

//INCLUDES
#include <cstring>
#include <iostream>
#include <iomanip>
#include <limits>
#include <cstdlib>		//exit() and EXIT_FAILURE

	//STL
#include <algorithm>
#include <map>
#include <vector>


//CUSTOM VARS
enum HttpMethod { GET = 1, POST, DELETE, UNKNOWN};

#define MAX_CONNctions 100



