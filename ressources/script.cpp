/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   script.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adidion <adidion@student.s19.be>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/03 14:23:49 by adidion           #+#    #+#             */
/*   Updated: 2022/05/03 16:07:24 by adidion          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <iterator>
#include <vector>

int main(int ac, char **av, char **env)
{
	std::string a = "Content-type:text/html\r\n\r\n\
	<html>\n\
	<head>\n\
	<title>Hi !</title>\n\
	</head>\n\
	<body>\n";
	int i = 0;
	for (; env[i]; i++)
	{
		i++;
	}
	std::vector<std::string> v(env, env + i);
		std::cerr << "coucou" << std::endl;
	i = 0;
	unsigned int j = 0;
	for (; j < v.size(); j++)
	{
		if ((i = v.at(j).find("fname")) != std::string::npos)
			break ;
	}
	std::cerr << "here" << std::endl;
	if (i != std::string::npos && j < v.size())
	{
		std::string cgi(v.at(j).begin() + (v.at(j).find("fname") + 1), v.at(j).end());
		a += ("<h1>Hi " + cgi + " !</h1>");
	}
	else
	{
		a += "<h1>You don't have a name ?</h1>";
	}
	a += "</body></html>";
	std::cout << a << std::endl;
//	catch(const std::out_of_range& e)
//	{
//		std::cerr << e.what() << '\n';
//	}
	return (0);
}