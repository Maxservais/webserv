#include "webserv.hpp"

Response::Response(Request &request, std::string path, std::string default_page, std::string error_404, int port) : req(request), path(path), default_page(default_page), error_404(error_404), port(port)
{
	(void)this->port; // thats for antoine
	return;
}

Response	&Response::operator=(const Response &obj)
{
	(void) obj;
	return (*this);
}

Response::~Response()
{
	return ;
}

bool Response::exists()
{
	const std::string &path = this->path + this->req.getFile();
	std::ifstream input_file(path);
	if (!input_file.is_open())
		return false;
	return true;
}

std::string Response::full_code(int code)
{
	std::string ret;
	switch (code)
	{
		case 200:
			ret = " 200 OK\n";
			break;
		case 202:
			ret = " 202 Accepted\n";
			break;
		case 204:
			ret = " 204 No Content\n";
			break;
		case 400:
			ret = " 400 Bad Request\n";
			break;
		case 401:
			ret = " 401 Unauthorized\n";
			break;
		case 403:
			ret = " 403 Forbidden\n";
			break;
		case 404:
			ret = " 404 Not Found\n";
			break;
		case 414:
			ret = " 414 URI Too Long\n";
			break;
	}
	return (ret);
}

std::string Response::content_length(std::string file)
{
	if (req.getFile_extention() == "png" || req.getFile_extention() == "jpg" || req.getFile_extention() == "ico")
		return ("Content-Length: " + body(file) + "\n\n");
	std::ifstream in(file.c_str());
	unsigned int i = 0;
	while (in.get() != EOF) i++; 
	in.close();
	return ("Content-Length: " + std::to_string(i) + "\n\n");
}

std::string Response::content_type()
{
	std::string extension;
	if (req.getMethod() == "GET")
	{
		if (req.getFile() == "/")
		{
			extension = this->default_page;
			extension.erase(0, extension.rfind("."));
			extension = extension.erase(0, 1);
		}
		else
			extension = req.getFile_extention();

		if (extension == "html")
			return ("Content-Type: text/html; charset=utf-8\n");
		else if ((extension == "png" || extension == "jpg" || extension == "ico") && exists())
			return ("Content-Type: image/" + extension + "\n");
	}
	else if (req.getMethod() == "POST" || (req.getMethod() == "DELETE" && !exists()))
		return ("Content-Type: text/plain\n");
	return ("Content-Type: text/html\n");
}

std::string Response::body(std::string file)
{
	if (req.getFile_extention() == "png" || req.getFile_extention() == "jpg" || req.getFile_extention() == "ico")
	{
		std::ifstream image(file);
		std::stringstream buffer_s;
		buffer_s << image.rdbuf();
		return (buffer_s.str());
	}
	else
	{
		std::ifstream input_file(file);
		if (!input_file.is_open())
		{
			std::cout << "Failed to open the requested ressource" << std::endl;
			exit(EXIT_FAILURE);
		}
		return std::string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
	}
}

std::string   	get_link(const std::string &dir_ent, std::string &dir_name, int port)
{
	std::stringstream ss;
	std::string a("localhost");
	if (dir_ent == ".")
	ss << "\t\t<p><a href=\"http://" + a + ":" <<\
		port << dir_name + "\">" + dir_ent + "</a></p>\n";
	else if (dir_ent == "..")
	{
		std::string str(dir_name);
		std::string::iterator it = str.begin() + str.rfind("/");
		std::string::iterator ite = str.end();
		str.erase(it, ite);
		ss << "\t\t<p><a href=\"http://" + a + ":" <<\
		port << str + "\">" + dir_ent + "</a></p>\n";
	}
	else
		ss << "\t\t<p><a href=\"http://" + a + ":" <<\
		port << dir_name + "/" + dir_ent + "\">" + dir_ent + "</a></p>\n";
	return (ss.str());
}

std::string Response::ft_try_dir(Request &request)
{
	std::string dir_name(request.getFile_clean());
	std::string ret;
	DIR *dir = opendir((this->path + request.getFile()).c_str());
	//if (dir == NULL)
	//{
	//	return ("HTTP/1.1 200 OK\nContent-Type: text/html; charset=utf-8\nContent-Length: 7650\n\n" + read_file_to_str("ressources/error404.html"));
	//}
	ret +=\
	"<!DOCTYPE html>\n\
	<html>\n\
	<head>\n\
	<title>" + dir_name + "</title>\n\
	</head>\n\
	<body>\n\
	<h1>AutoIndex of " + dir_name + "</h1>\n\
	<p>\n";	
	if (dir_name[0] != '/')
		dir_name = "/" + dir_name;
	for (struct dirent *dir_buff = readdir(dir); dir_buff; dir_buff = readdir(dir))
	{
		ret += get_link(std::string(dir_buff->d_name), dir_name, this->port);
	}
	ret +="\
	</p>\n\
	</body>\n\
	</html>\n";
	closedir(dir);
	return (ret);
}

std::string Response::compose_response()
{
	if (req.getMethod() == "GET")
	{
		if (req.getFile() == "/")
			this->response = req.getVersion() + full_code(200) + content_type() + content_length(this->path + "/" + this->default_page) + body(this->path + "/" + this->default_page);
		else if (req.getFile().size() > 1 && req.getFile_extention().empty())
		{
			if (!opendir((this->path + req.getFile()).c_str()))
				this->response = req.getVersion() + full_code(200) + content_type() + content_length(this->path + "/" + this->error_404) + body(this->path + "/" + this->error_404);
			else
				this->response = req.getVersion() + full_code(200) + content_type() + "Content-Length: 1000000\n\n" + ft_try_dir(req);
		}
		else if (exists())
			this->response = req.getVersion() + full_code(200) + content_type() + content_length(this->path + req.getFile()) + body(this->path + req.getFile());
		else
			this->response = req.getVersion() + full_code(200) + content_type() + content_length(this->path + "/" + this->error_404) + body(this->path + "/" + this->error_404);
	}
	else if (req.getMethod() == "POST")
		this->response = req.getVersion() + full_code(204) + content_type();
	else if (req.getMethod() == "DELETE")
	{
		if (exists())
		{
			remove((this->path + req.getFile()).c_str());
			this->response = req.getVersion() + full_code(200) + content_type() + content_length(this->path + "/file.html") + body(this->path + "/file.html");
		}
		else
			this->response = req.getVersion() + full_code(204) + content_type();
	}
	else
		this->response = req.getVersion() + full_code(204) + content_type();
 	return this->response;
}

std::string Response::get_response()
{
	std::cout << compose_response() << std::endl;
	return (compose_response());
}
