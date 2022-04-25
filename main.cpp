#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <vector>

std::string convert_to_binary(const char * path)
{
	std::vector<char> buffer;
	FILE * file_stream = fopen(path, "rb");
	int file_size;

	if (file_stream != nullptr)
	{
		fseek(file_stream, 0, SEEK_END); // https://www.javatpoint.com/fseek-in-c The fseek() function is used to set the file pointer to the specified offset. It is used to write data into file at desired location.
		long file_length = ftell(file_stream); // ftell() in C is used to find out the position of file pointer in the file with respect to starting of the file.
		rewind(file_stream); // The rewind() function sets the file position indicator for the stream pointed to by stream to the beginning of the file.

		buffer.resize(file_length);

		file_size = fread(&buffer[0], 1, file_length, file_stream);
	}

	std::string file_str(buffer.begin(), buffer.end());
	std::string file_size_str = std::to_string(file_str.length());
	// std::string ret = "HTTP/1.1 200 OK\nContent-Type: image/png; \nContent-Length: " + file_size_str + file_str + "/n/n";
	std::string ret = "HTTP/1.1 200 OK\r\nContent-Type: image/png; Content-Transfer-Encoding: binary; Content-Length: " + file_size_str + ";charset=ISO-8859-4 \r\n\r\n" + file_str;
	return (ret);
}

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
	connection = accept(sockfd, (struct sockaddr*)&sockaddr, &addrlen);
	if (connection < 0)
	{
		std::cout << "Failed to grab connection!" << std::endl;
		return (EXIT_FAILURE);
	}

	char	buffer_2[1000000];
	read(connection, buffer_2, 1000000);
	std::cout << "Message was for second request: " << buffer_2 << std::endl;

	const char * path = "ball.png";
	response = convert_to_binary(path);
	send(connection, response.c_str(), response.size(), 0);

	/* Close the connections */
	close(connection);
	close(sockfd);
	// close(fd);

	return (EXIT_SUCCESS);
}
