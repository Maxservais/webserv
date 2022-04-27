/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adidion <adidion@student.s19.be>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/26 17:12:02 by adidion           #+#    #+#             */
/*   Updated: 2022/04/27 14:52:02 by adidion          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include "Log.hpp"

int main()
{
	Log log;
	char a[1000000] = "    POST png HTTP/1.1";
	std::cout << "hi" << std::endl;
	Request b(a);
	std::cout << b.getMethod() << std::endl;
	std::cout << b.getFile() << std::endl;
	std::cout << b.getFile_clean() << std::endl;
	std::cout << b.getVersion() << std::endl;
	std::cout << b.getFile_extention() << std::endl;
	log.add_one(b);

	char x[1000000] = "POST html HTTP/1.1\nHost: localhost:9999";
	Request c(x);
	std::cout << c.getMethod() << std::endl;
	std::cout << c.getFile() << std::endl;std::cout << c.getFile_clean() << std::endl;
	std::cout << c.getVersion() << std::endl;
	std::cout << c.getFile_extention() << std::endl;
	log.add_one(c);
}
