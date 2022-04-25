#include <sys/socket.h> // socket functions
#include <netinet/in.h> // socket functions
#include <stdlib.h>
#include <unistd.h> // read & write in C
#include <iostream> // input/output streams
#include <fstream> // file streams
#include <sstream> // std::stringstream
#include <fcntl.h> // open in C


int	main(void)
{
	int			sockfd;
	sockaddr_in	sockaddr;

	/* Create a socket (IPv4, TCP) */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		std::cout << "Failed to create socket!" << std::endl;
		return (EXIT_FAILURE);
	}

	/* Assign an IP address and port to the socket */
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.s_addr = INADDR_ANY;
	sockaddr.sin_port = htons(8080); // convert a number to the network byte order
	if (bind(sockfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0)
	{
		std::cout << "Failed to bind !" << std::endl;
		return (EXIT_FAILURE);
	}

	/* Start listening (max 10 connections in the queue) */
	if (listen(sockfd, 10) < 0)
	{
		std::cout << "Failed to listen on socket!" << std::endl;
		return (EXIT_FAILURE);
	}

	/* Grab a connection from the queue */
	socklen_t addrlen = sizeof(sockaddr);
	int	connection = accept(sockfd, (struct sockaddr*)&sockaddr, &addrlen);
	if (connection < 0)
	{
		std::cout << "Failed to grab connection!" << std::endl;
		return (EXIT_FAILURE);
	}

	/* Read from the connection and print output */
	char	buffer[1000000];
	read(connection, buffer, 1000000);
	std::cout << "Message was: " << buffer << std::endl;

	/* Send a message to the connection */
	std::string response = "HTTP/1.1 200 OK\nContent-Type: text/html; charset=utf-8\nContent-Length: 7650\n\n<!DOCTYPE html><html> <head><meta charset=\"utf-8\">server Response</head><body><h1> This page was render direcly from the server <p>Hello there welcome to my website</p></h1><img src=\"ball.png\"></body></html>";
	send(connection, response.c_str(), response.size(), 0);
	close(connection);

	/* Grab another connection from the queue */
	connection = accept(sockfd, (struct sockaddr*)&sockaddr, &addrlen);
	if (connection < 0)
	{
		std::cout << "Failed to grab connection!" << std::endl;
		return (EXIT_FAILURE);
	}

	/* Read from the connection and print output */
	char	buffer_2[1000000];
	read(connection, buffer_2, 1000000);
	std::cout << "Message was for second request: " << buffer_2 << std::endl;

	std::ifstream image("ball.png");
	std::stringstream buffer_s;
	buffer_s << image.rdbuf();

	/* Send response to client */
	response = "HTTP/1.1 200 OK\nContent-Type: image/png; \nContent-Length: 105345\n\n" + buffer_s.str();
	send(connection, response.c_str(), response.size(), 0);

	/* Close the connections */
	close(connection);
	close(sockfd);

	return (EXIT_SUCCESS);
}
