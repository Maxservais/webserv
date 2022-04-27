/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adidion <adidion@student.s19.be>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/26 16:25:31 by adidion           #+#    #+#             */
/*   Updated: 2022/04/27 11:36:32 by adidion          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <vector>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <iterator>

class	Request
{
	private:
		std::string buff;
	public:
		Request();
		Request(std::string &buffer);
		Request(char *buffer);
		Request(const Request &obj);
		Request &operator=(const Request &objz);
		~Request();
		std::string getBuff() const;
		std::string getMethod();
		std::string getFile();
		std::string getVersion();
		std::vector<std::string> split_words(std::string buffer);
};

#endif
