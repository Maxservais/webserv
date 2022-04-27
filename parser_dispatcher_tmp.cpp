#include "webserv.hpp"

std::string read_file_to_str(const std::string& path)
{
	std::ifstream input_file(path);
	if (!input_file.is_open())
	{
		std::cout << "Failed to open the requested ressource" << std::endl;
		exit(EXIT_FAILURE);
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
		std::cout << "Failed to open the requested ressource" << std::endl;
		exit(EXIT_FAILURE);
	}
	stream.seekg(0, std::ios_base::end);
	long file_length = stream.tellg();
	stream.seekg(0);

	buffer.resize(file_length);
	stream.read(&buffer[0], file_length);

	std::string file_str(buffer.begin(), buffer.end());
	stream.close();
	return (file_str);
}

std::vector<std::string> parser(char * buffer)
{
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
	return(vector);
}

std::string dispatcher(std::vector<std::string> vector)
{
	std::string response;
	if (vector[0] == "GET")
	{
		if (vector[1] == "/index.html" || vector[1] == "/")
			response = "HTTP/1.1 200 OK\nContent-Type: text/html; charset=utf-8\nContent-Length: 7650\n\n" + read_file_to_str("ressources/index.html");
		else if (vector[1] == "/ball.png" || vector[1] == "/favicon.ico")
		{
			const char * path = "ressources/ball.png";
			response = convert_to_binary(path);
			std::string response_len = std::to_string(response.length());
			std::string ret = "HTTP/1.1 200 OK\r\nContent-Type: image/png; Content-Transfer-Encoding: binary; Content-Length: " + response_len + ";charset=ISO-8859-4 \r\n\r\n" + response;
		}
		else if (vector[1] == "/page.html")
			response = "HTTP/1.1 200 OK\nContent-Type: text/html; charset=utf-8\nContent-Length: 7650\n\n" + read_file_to_str("ressources/page.html");
		else 
			response = "HTTP/1.1 200 OK\nContent-Type: text/html; charset=utf-8\nContent-Length: 7650\n\n" + read_file_to_str("ressources/error404.html");
	}

	else if (vector[0] == "DELETE")
	{
		if ( vector[1] == "/ball.png" || vector[1] == "/page.html" || vector[1] == "/index.html")
		{
			std::string name = "ressources" + vector[1];
			const char * file_name = const_cast<char*>(name.c_str());
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
	return (response);
}
