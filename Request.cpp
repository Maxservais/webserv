/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adidion <adidion@student.s19.be>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/26 16:43:47 by adidion           #+#    #+#             */
/*   Updated: 2022/04/27 14:17:37 by adidion          ###   ########.fr       */
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
	(void) obj;//*this = obj;
	return (*this);
}

Request::~Request()
{
	return ;
}

// split a string with spaces without care of allocations and returns it into a vector

std::vector<std::string> Request::split_words(std::string s)
{
	std::stringstream ss(s);
	std::istream_iterator<std::string> begin(ss);
	std::istream_iterator<std::string> end;
	return (std::vector<std::string>(begin, end));
}

//return buff in private
std::string Request::getBuff() const
{
	return (buff);
}
//return a std::string of the method in capital letters
//if the method doesn't exist, it returns a clear std:::string
std::string Request::getMethod()
{
	std::vector<std::string> v = split_words(buff);
	if (v.empty() == 1)
		return (std::string());
	if (v.at(0) == "GET" || v.at(0) == "POST" || v.at(0) == "DELETE")
		return (v.at(0));
	return (std::string());
}

// return the path of the file, if it exists
// else, it return only a "/"

std::string Request::getFile()
{
	std::vector<std::string> v = split_words(buff);
	if (v.size() < 2)
		return (std::string("/"));
	return (v.at(1));
}

//return the HTTP version of the request (ex : HTTP/1.1)
// in case of error, it returns an empty string ("")
std::string Request::getVersion()
{
	std::vector<std::string> v = split_words(buff);
	if (v.size() < 3)
		return (std::string());
	return (v.at(2));
}

std::string Request::getFile_clean()
{
	std::vector<std::string> v = split_words(buff);
	if (v.size() < 2)
		return (std::string());
	return (v.at(1));
}
