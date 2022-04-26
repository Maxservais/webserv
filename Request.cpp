/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adidion <adidion@student.s19.be>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/26 16:43:47 by adidion           #+#    #+#             */
/*   Updated: 2022/04/26 18:25:39 by adidion          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include <cctype>

Request::Request()
{
	return ;
}

Request::Request(std::string &buffer): buff(buffer)
{
	return ;
}

Request::Request(char *buffer): buff(buffer)
{
	return ;
}

Request::Request(const Request &obj)
{
	*this = obj;
}

Request	&Request::operator=(const Request &obj)
{
	*this = obj;
	return (*this);
}

Request::~Request()
{
	return ;
}
//return buff in private
std::string Request::getBuff() const
{
	return (buff);
}
//return a std::string of the method in capital letters
//if the method doesn't exist, it returns a clear std:::string
std::string Request::getMethod() const
{
	unsigned int i = -1;
	while (buff.at(++i) && isspace(static_cast<int>(buff.at(i))));
	if (buff.at(i))
	{
		if (buff.find("GET ", i) != std::string::npos)
			return (std::string("GET"));
		if (buff.find("POST ", i) != std::string::npos)
			return (std::string("POST"));
		if (buff.find("DELETE ", i) != std::string::npos)
			return (std::string("DELETE"));
	}
	return (std::string());

}

// return the path of the file, if it exists
// else, it return only a "/"

std::string Request::getFile() const
{
	unsigned int i = -1;
	while (buff.at(++i) && isspace(static_cast<int>(buff.at(i))));
	if (buff.at(i))
	{
		while (buff.at(++i) && !isspace(static_cast<int>(buff.at(i))));
		if (buff.at(i))
			while (buff.at(++i) && isspace(static_cast<int>(buff.at(i))));
	}
	if (buff.at(i))
	{
		std::string::const_iterator it = buff.begin(), ite = it;
		it = it + i;
		while (buff.at(++i) && !isspace(static_cast<int>(buff.at(i))));
		ite = ite + i;
		return (std::string(it, ite));
	}
	return (std::string());

}
