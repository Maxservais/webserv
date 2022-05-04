/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   script.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adidion <adidion@student.s19.be>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/03 14:23:49 by adidion           #+#    #+#             */
/*   Updated: 2022/05/04 12:02:12 by adidion          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <iterator>
#include <vector>

std::string &addfname(std::vector<std::string> &v, std::string &a, unsigned int j, unsigned int i)
{
	std::string cgi(v.at(j).begin() + (v.at(j).find("fname") + 6), v.at(j).end());
	if (cgi == "admin")
		a += ("<h1>Hey boss, glad to see you here !</h1><br>");
	else if (cgi == "")
		a += "<h1>Don't you have a name ?</h1><br>";
	else
		a += ("<h1>Hi " + cgi + " !</h1><br>");
	return (a);
}

std::string &addsname(std::vector<std::string> &v, std::string &a, unsigned int j, unsigned int i)
{
	std::string cgi(v.at(j).begin() + (v.at(j).find("sname") + 6), v.at(j).end());
	if (cgi == "admin")
		a += ("<h1>I totally respect you !</h1>");
	else if (cgi == "")
		a += "<h1>Don't you have a second name name ?</h1>";
	else
		a += ("<h1>Your second name is " + cgi + " !</h1>");
	return (a);
}

int main(int ac, char **av, char **env)
{
	std::string a = "<html>\n\
	<head>\n\
	<title>Hi !</title>\n\
	</head>\n\
	<body>\n";
	int i = 0;
	int boo = 0;
	for (; env[i]; i++)
	{
		;
	}
	std::vector<std::string> v(env, env + i);
	i = 0;
	unsigned int j = 0;
	for (; j < v.size(); j++)
	{
		if ((i = v.at(j).find("fname")) != std::string::npos)
		{
			addfname(v, a, j, i);
			break ;
		}
	}
	if (j == v.size())
	{
		a += "<h1>Don't you have a name ?</h1>";
	}
	j = 0;
	for (; j < v.size(); j++)
		if ((i = v.at(j).find("sname")) != std::string::npos)
		{
			addsname(v, a, j, i);
			break ;
		}
	if (j == v.size())
	{
		a += "<h1>Don't you have a name ?</h1>";
	}
	a += "</body></html>";
	std::cout << a << std::endl;
	return (0);
}