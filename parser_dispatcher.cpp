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

std::string get_length_file(std::string file)
{
	std::ifstream in(file.c_str());
	unsigned int i = 0;
	while (in.get() != EOF) i++; 
	in.close();
	return std::to_string(i);
}

std::string image_treatment(std::string path)
{
	std::ifstream image(path);
	std::stringstream buffer_s;
	buffer_s << image.rdbuf();
	return (buffer_s.str());
}

bool exists(const std::string& path)
{
	std::ifstream input_file(path);
	if (!input_file.is_open())
		return false;
	return true;
}

std::string dispatcher(Request &request)
{
	std::string response;
	if (request.getMethod() == "GET")
	{
		if (request.getFile() == "/")
			response = "HTTP/1.1 200 OK\nContent-Type: text/html; charset=utf-8\nContent-Length: " + get_length_file("ressources/index.html") + "\n\n" + read_file_to_str("ressources/index.html");
		if (exists("ressources" + request.getFile()))
		{
			if (request.getFile_extention() == "html")
				response = "HTTP/1.1 200 OK\nContent-Type: text/html; charset=utf-8\nContent-Length: " + get_length_file("ressources" + request.getFile()) + "\n\n" + read_file_to_str("ressources" + request.getFile());
			else if (request.getFile_extention() == "png" || request.getFile_extention() == "ico" || request.getFile_extention() == "jpg")
			{
				response = image_treatment("ressources" + request.getFile());
				response = "HTTP/1.1 200 OK\r\nContent-Type: image/png; Content-Transfer-Encoding: binary; Content-Length: " + std::to_string(response.length()) + ";charset=ISO-8859-4 \r\n\r\n" + response;
			}
		}
		else
			response = "HTTP/1.1 200 OK\nContent-Type: text/html; charset=utf-8\nContent-Length: 7650\n\n" + read_file_to_str("ressources/error404.html");
	}
	else if (request.getMethod() == "POST")
		response = "HTTP/1.1 204 No Content\n\nContent-Type: text/plain\n";
	else if (request.getMethod() == "DELETE")
	{
		std::string composed = "ressources" + request.getFile();
		if (remove(composed.c_str()) != 0)
				response = "HTTP/1.1 204 No Content\n\nContent-Type: text/plain\n";
		else
				response = "HTTP/1.1 200 OK\nContent-Type: text/html; charset=utf-8\nContent-Length: 48\n\n<html><body><h1>File deleted.</h1></body></html>";
	}
	return (response);
}