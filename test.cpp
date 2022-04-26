/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adidion <adidion@student.s19.be>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/26 17:12:02 by adidion           #+#    #+#             */
/*   Updated: 2022/04/26 18:22:30 by adidion          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

int main()
{
	char a[10000] = "    POST /ball.png HTTP/1.1";
	std::cout << "hi" << std::endl;
	Request b(a);
	std::cout << b.getMethod() << std::endl;
	std::cout << b.getFile() << std::endl;
}