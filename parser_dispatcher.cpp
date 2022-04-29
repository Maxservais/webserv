#include "webserv.hpp"
//#include <sys/types.h>
#include <dirent.h>

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

std::string convert_to_binary(std::string path)
{
	std::vector<char> buffer;
	std::ifstream stream;
	stream.open(path.c_str(), std::ifstream::binary);
	if (!stream.is_open())
	{
		std::cout << "Failed to open the requested ressource in convert to binary" << std::endl;
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

bool exists (Request request)
{
	if (request.getFile() == "/")
		return true;
	std::ifstream input_file("ressources" + request.getFile());
	if (!input_file.is_open())
		return false;
	else
		return true;
}

bool exists(const std::string& path)
{
	std::ifstream input_file(path);
	if (!input_file.is_open())
		return false;
	return true;
}

std::string	get_link(std::string const &dir_ent, std::string const &dir_name, int port)
{
	std::stringstream ss;
	std::string a("localhost");
	ss << "\t\t<p><a href=\"http://" + a + ":" <<\
		port << dir_name + "/" + dir_ent + "\">" + dir_ent + "</a></p>\n";
	return (ss.str());
}

std::string ft_try_dir(Request &request)
{
	std::string dir_name(request.getFile());
	std::string ret;
	DIR *dir = opendir(("ressources" + request.getFile()).c_str());
	if (dir == NULL)
	{
		return ("HTTP/1.1 200 OK\nContent-Type: text/html; charset=utf-8\nContent-Length: 7650\n\n" + read_file_to_str("ressources/error404.html"));
	}
	ret +=\
	"<!DOCTYPE html>\n\
	<html>\n\
	<head>\n\
		<title>" + dir_name + "</title>\n\
	</head>\n\
	<body>\n\
	<h1>INDEX</h1>\n\
	<p>\n";	
	if (dir_name[0] != '/')
		dir_name = "/" + dir_name;
	for (struct dirent *dir_buff = readdir(dir); dir_buff; dir_buff = readdir(dir))
	{
		ret += get_link(std::string(dir_buff->d_name), dir_name, 9999);
	}
	ret +="\
	</p>\n\
	</body>\n\
	</html>\n";
	closedir(dir);
	std::cout << ret << std::endl;
	return (ret);
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
			std::cout << "a" << std::endl;
			if (request.getFile_extention() == "html")
				response = "HTTP/1.1 200 OK\nContent-Type: text/html; charset=utf-8\nContent-Length: " + get_length_file("ressources" + request.getFile()) + "\n\n" + read_file_to_str("ressources" + request.getFile());
			else if (request.getFile_extention() == "png" || request.getFile_extention() == "ico" || request.getFile_extention() == "jpg")
			{
				response = convert_to_binary("ressources" + request.getFile());
				std::string response_len = std::to_string(response.length());
				response = "HTTP/1.1 200 OK\r\nContent-Type: image/png; Content-Transfer-Encoding: binary; Content-Length: " + response_len + ";charset=ISO-8859-4 \r\n\r\n" + response;
			}
			else if (request.getFile() != "/")
				return (ft_try_dir(request));
		}
		else
			response = "HTTP/1.1 200 OK\nContent-Type: text/html; charset=utf-8\nContent-Length: 7650\n\n" + read_file_to_str("ressources/error404.html");
	}
	else if (request.getMethod() == "POST")
	{
		response = "HTTP/1.1 204 No Content\n\nContent-Type: text/plain\n";
		// faudra parser la reposne cgi
	}
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