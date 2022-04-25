#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>


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
		std::cout << "Failed to bind to port 8080!" << std::endl;
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

	// while (1)
	// {
		/* Read from the connection and print output */
		char	buffer[1000];
		read(connection, buffer, 1000);
		std::cout << "Message was: " << buffer << std::endl;

		/* Send a message to the connection */
		std::string response = "HTTP/1.1 200 OK\nContent-Type: text/html; charset=utf-8\nContent-Length: 148\n\n<!DOCTYPE html><html><head><meta charset=\"utf-8\"><title>HTTP Headers</title></head><body><h1>HTTP Headers</h1><img src=\"ball.png\" alt=\"nnaaaaaah\"></body></html>";
		send(connection, response.c_str(), response.size(), 0);
	// }
	/* Close the connections */
	close(connection);
	close(sockfd);
	return (EXIT_SUCCESS);
}
