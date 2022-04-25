#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <vector>

std::string read_file_to_str(const std::string& path)
{
	std::ifstream input_file(path);
	if (!input_file.is_open()) 
	{
		std::cout << "Failed to open the requested ressource" << std::endl;
	}
	return std::string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
}

std::string convert_to_binary(const char * path)
{
	std::vector<char> buffer;
	std::ifstream stream;
	stream.open(path, std::ifstream::binary);
	if (!stream.is_open())
	{
		std::cerr << path << " failed to open." << std::endl;
		exit(EXIT_FAILURE);
	}
	stream.seekg(0, std::ios_base::end);
	long file_length = stream.tellg();
	stream.seekg(0);

	buffer.resize(file_length);
	stream.read(&buffer[0], file_length); // /!\ a proteger, je regarde ca

	std::string file_str(buffer.begin(), buffer.end());
	std::string file_size_str = std::to_string(file_str.length());
	std::string ret = "HTTP/1.1 200 OK\r\nContent-Type: image/png; Content-Transfer-Encoding: binary; Content-Length: " + file_size_str + ";charset=ISO-8859-4 \r\n\r\n" + file_str;
	stream.close();
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

	char	buffer[1000000];
	while(1)
	{
		socklen_t addrlen = sizeof(sockaddr);

		/* HEADERS */
		int	connection = accept(sockfd, (struct sockaddr*)&sockaddr, &addrlen);
		if (connection < 0)
		{
			std::cout << "Failed to grab connection!" << std::endl;
			return (EXIT_FAILURE);
		}

		std::cout << buffer << std::endl;
		memset(buffer, 0, 1000000);
		read(connection, buffer, 1000000);
		std::cout << buffer << std::endl;

		std::string response = "HTTP/1.1 200 OK\nContent-Type: text/html; charset=utf-8\nContent-Length: 7650\n\n" + read_file_to_str("index.html");
		send(connection, response.c_str(), response.size(), 0);

		close(connection);

		/* IMAGE */
		connection = accept(sockfd, (struct sockaddr*)&sockaddr, &addrlen);
		if (connection < 0)
		{
			std::cout << "Failed to grab connection!" << std::endl;
			return (EXIT_FAILURE);
		}

		memset(buffer, 0, 1000000);
		read(connection, buffer, 1000000);
		std::cout << buffer << std::endl;

		const char * path = "ball.png";
		response = convert_to_binary(path);
		send(connection, response.c_str(), response.size(), 0);

		close(connection);

		/* POST */
		connection = accept(sockfd, (struct sockaddr*)&sockaddr, &addrlen);
		if (connection < 0)
		{
			std::cout << "Failed to grab connection!" << std::endl;
			return (EXIT_FAILURE);
		}
		memset(buffer, 0, 1000000);
		read(connection, buffer, 1000000);
		std::cout << buffer << std::endl;

		/* LIEN */
		int connection_2 = accept(sockfd, (struct sockaddr*)&sockaddr, &addrlen);
		if (connection_2 < 0)
		{
			std::cout << "Failed to grab connection_2!" << std::endl;
			return (EXIT_FAILURE);
		}

		memset(buffer, 0, 1000000);
		read(connection_2, buffer, 1000000);
		std::cout << buffer << std::endl;
		response = "HTTP/1.1 200 OK\nContent-Type: text/html; charset=utf-8\nContent-Length: 7650\n\n" + read_file_to_str("page.html");
		send(connection_2, response.c_str(), response.size(), 0);
		close(connection_2);
	}

	close(sockfd);
	return (EXIT_SUCCESS);
}
