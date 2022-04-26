
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <vector>
#include <cstdio>

#define SERVER_PORT 9999

std::vector<std::string> split_file_name(std::string name)
{
	std::vector<std::string> splitted_file_name;
	int pos = name.find_first_of('.');
	splitted_file_name.push_back(name.substr(pos + 1));
	splitted_file_name.push_back(name.substr(0,pos));
	return splitted_file_name;
}

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
	stream.close();
	return (file_str);
}

int main()
{
	int			sockfd;
	sockaddr_in	sockaddr;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		std::cout << "Failed to create socket!" << std::endl;
		return (EXIT_FAILURE);
	}

	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.s_addr = INADDR_ANY;
	sockaddr.sin_port = htons(SERVER_PORT); // convert a number to the network byte order
	if (bind(sockfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0)
	{
		std::cout << "Failed to bind !" << std::endl;
		return (EXIT_FAILURE);
	}

	if (listen(sockfd, 10) < 0)
	{
		std::cout << "Failed to listen on socket!" << std::endl;
		return (EXIT_FAILURE);
	}

	char	buffer[1000000];
	while(1)
	{
		socklen_t addrlen = sizeof(sockaddr);

		int	connection = accept(sockfd, (struct sockaddr*)&sockaddr, &addrlen);
		if (connection < 0)
		{
			std::cout << "Failed to grab connection!" << std::endl;
			return (EXIT_FAILURE);
		}

		memset(buffer, 0, 1000000);
		read(connection, buffer, 1000000);
		std::cout << buffer << std::endl;

		std::vector<std::string> vector;
		std::string a;
		std::string b;
		int i = 0;
		while(buffer[i] != ' ')
			a.push_back(buffer[i++]);
		i++;
		while(buffer[i] != ' ')
			b.push_back(buffer[i++]);
		vector.push_back(a);
		vector.push_back(b);

		std::string response;
		if (vector[0] == "GET")
		{
			if (vector[1] == "/index.html" || vector[1] == "/")
				response = "HTTP/1.1 200 OK\nContent-Type: text/html; charset=utf-8\nContent-Length: 7650\n\n" + read_file_to_str("index.html");
			else if (vector[1] == "/ball.png" || vector[1] == "/favicon.ico")
			{
				const char * path = "ball.png";
				response = convert_to_binary(path);
				std::string response_len = std::to_string(response.length());
				std::string ret = "HTTP/1.1 200 OK\r\nContent-Type: image/png; Content-Transfer-Encoding: binary; Content-Length: " + response_len + ";charset=ISO-8859-4 \r\n\r\n" + response;
			}
			else if (vector[1] == "/page.html")
				response = "HTTP/1.1 200 OK\nContent-Type: text/html; charset=utf-8\nContent-Length: 7650\n\n" + read_file_to_str("page.html");
			else 
				response = "HTTP/1.1 200 OK\nContent-Type: text/html; charset=utf-8\nContent-Length: 7650\n\n" + read_file_to_str("error404.html");
			// send(connection, response.c_str(), response.size(), 0);
		}

		else if (vector[0] == "DELETE")
		{
			if ( vector[1] == "/ball.png" || vector[1] == "/page.html" || vector[1] == "/index.html")
			{
				vector[1].erase(0,1);
				const char * file_name = const_cast<char*>(vector[1].c_str());
				if (remove(file_name) != 0)
					response = "HTTP/1.1 204 No Content\n\nContent-Type: text/plain\n";
				else
					response = "HTTP/1.1 200 OK\nContent-Type: text/html; charset=utf-8\nContent-Length: 48\n\n<html><body><h1>File deleted.</h1></body></html>";
			}
			else
				response = "HTTP/1.1 204 No Content\n\nContent-Type: text/plain\n";
		}

		else if (vector[0] == "POST")
		{
			std::cout << "I AM A POST" << std::endl;
			response = "HTTP/1.1 204 No Content\n\nContent-Type: text/plain\n";
		}
		else
		{
			std::cout << "NOT A POST OR GET" << std::endl;
			response = "HTTP/1.1 204 No Content\n\nContent-Type: text/plain\n";
		}
		send(connection, response.c_str(), response.size(), 0);
		close(connection);
	}
	close(sockfd);
	return (EXIT_SUCCESS);
}
