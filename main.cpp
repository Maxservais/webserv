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
		// std::string response = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
		std::string response = "HTTP/1.1 200 OK\nContent-Type: text/html; charset=utf-8\nContent-Length: 7650\n\n<!DOCTYPE html><html> <head><meta charset=\"utf-8\">server Response</head><body><h1> This page was render direcly from the server <p>Hello there welcome to my website</p></h1><img src=\"data:image/jpeg;base64,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\"></body></html>";
		// std::string response = "HTTP/1.1 200 OK\nContent-Type: text/html; charset=utf-8\nContent-Length: 133\n\n<!DOCTYPE html><html><head><meta charset=\"utf-8\"><title>HTTP Headers</title></head><body><h1>HTTP Headers</h1></body></html>";
		// std::string response = "HTTP/1.1 200 OK\nContent-Type: text/html; charset=utf-8\nContent-Length: 148\n\n<!DOCTYPE html><html><head><meta charset=\"utf-8\"><title>HTTP Headers</title></head><body><h1>HTTP Headers</h1><img src=\"ball.png\" alt=\"nnaaaaaah\"></body></html>";
		// std::string response_2 = "HTTP/1.1 200 OK\nContent-Type: image/png\nContent-Length: 105345\n\n";
		send(connection, response.c_str(), response.size(), 0);
		//send(connection, response_2.c_str(), response_2.size(), 0);
	// }
	/* Close the connections */
	close(connection);
	close(sockfd);
	return (EXIT_SUCCESS);
}
