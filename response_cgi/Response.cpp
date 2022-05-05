#include "../webserv.hpp"

/* ************************************************************************** */
/*  CANON                                                                     */
/* ************************************************************************** */
Response::Response(Request &request, Config &config, std::string error_404) : req(request), config(config), error_404(error_404)
{
	this->server_index = 0;
	std::string tmp = request.getHost();
	size_t pos = tmp.find(":");
	int port_tmp = atoi(tmp.substr(pos + 1).c_str());

	for(size_t i = 0; i < this->config.get_servers().size(); i++)
	{
		if (this->config.get_servers()[i]->get_port() == port_tmp)
			this->server_index = i;
	}
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

/* ************************************************************************** */
/*  UTILS                                                                     */
/* ************************************************************************** */
bool Response::exists()
{
	const std::string &path = this->config.get_servers()[server_index]->get_root() + this->req.getFile();
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
		case 204:
			ret = " 204 No Content\n";
			break;
		case 404:
			ret = " 404 Not Found\n";
			break;
		case 501:
			ret = " 501 Not Implemented\n";
			break;
	}
	return (ret);
}

/* ************************************************************************** */
/*  RESPONSE COMPOSITION                                                      */
/* ************************************************************************** */
std::string Response::content_length(std::string file, int hint)
{
	if(hint == IS_DIR)
		return ("Content-Length: " + std::to_string(ft_try_dir(req).size()) + "\n\n");

	if (hint == IS_CGI)
		return ("Content-Length: " + std::to_string(file.size()) + "\n\n");

	if (req.getFile_extention() == "png" || req.getFile_extention() == "jpg" || req.getFile_extention() == "ico" || 
		req.getFile_extention() == "gif" || req.getFile_extention() == "webp")
		return ("Content-Length: " + std::to_string(body(file).size()) + "\n\n");

	std::ifstream in(file.c_str());
	unsigned int i = 0;
	while (in.get() != EOF) i++; 
	in.close();
	return ("Content-Length: " + std::to_string(i) + "\n\n");
}

std::string Response::content_type()
{
	if (req.getFile_extention() == "cgi")
		return ("Content-Type: text/html\n");

	if (req.getMethod() == "GET")
	{
		std::string extension;
		if (req.getFile() == "/")
		{
			extension = this->config.get_servers()[server_index]->get_index();
			extension.erase(0, extension.rfind("."));
			extension = extension.erase(0, 1);
		}

		else
			extension = req.getFile_extention();

		if (extension == "html")
			return ("Content-Type: text/html; charset=utf-8\n");

		else if ((req.getFile_extention() == "png" || req.getFile_extention() == "jpg" || req.getFile_extention() == "ico" || 
		req.getFile_extention() == "gif" || req.getFile_extention() == "webp") && exists())
		{
			return ("Content-Type: image/" + extension + "\n");
		}
	}


	else if (req.getMethod() == "POST" || (req.getMethod() == "DELETE" && !exists()))
	{
		return ("Content-Type: text/plain\n");
	}

	return ("Content-Type: text/html\n");
}

std::string Response::body(std::string file)
{
	if (req.getFile_extention() == "png" || req.getFile_extention() == "jpg" || req.getFile_extention() == "ico" ||
	req.getFile_extention() == "gif" || req.getFile_extention() == "webp") // gif et webp pas testeee
	{
		std::ifstream image(file);
		std::stringstream buffer_s;
		buffer_s << image.rdbuf();
		return (buffer_s.str());
	}

	else
	{
		std::ifstream input_file(file);
		return std::string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
	}
}

/* ************************************************************************** */
/*  AUTo INDEX                                                                */
/* ************************************************************************** */
std::string   	get_link(const std::string &dir_ent, std::string &dir_name, int port)
{
	std::stringstream ss;
	std::string a("localhost");
	if (dir_ent == ".")
	ss << "<p><a href=\"http://" + a + ":" << port << dir_name + "\">" + dir_ent + "</a></p>\n";
	else if (dir_ent == "..")
	{
		std::string str(dir_name);
		std::string::iterator it = str.begin() + str.rfind("/");
		std::string::iterator ite = str.end();
		str.erase(it, ite);
		ss << "<p><a href=\"http://" + a + ":" << port << str + "\">" + dir_ent + "</a></p>\n";
	}
	else
		ss << "<p><a href=\"http://" + a + ":" << port << dir_name + "/" + dir_ent + "\">" + dir_ent + "</a></p>\n";
	return (ss.str());
}

std::string Response::ft_try_dir(Request &request)
{
	std::string dir_name(request.getFile_clean());
	std::string ret;
	DIR *dir = opendir((this->config.get_servers()[server_index]->get_root() + request.getFile()).c_str());
	if (dir == NULL)
		return ("");
	ret +=\
	"<!DOCTYPE html>\n<html>\n<head>\n<title>" + dir_name + "</title>\n</head>\n<body>\n<h1>AutoIndex of " + dir_name + "</h1>\n<p>\n";	
	if (dir_name[0] != '/')
		dir_name = "/" + dir_name;
	for (struct dirent *dir_buff = readdir(dir); dir_buff; dir_buff = readdir(dir))
	{
		ret += get_link(std::string(dir_buff->d_name), dir_name, this->config.get_servers()[server_index]->get_port());
	}
	ret +="</p>\n</body>\n</html>\n";
	closedir(dir);
	return (ret);
}

/* ************************************************************************** */
/*  CGI                                                                       */
/* ************************************************************************** */
std::string Response::html_code_cgi(Request &req)
{
	Cgi a(req);
	return (a.executeCgi());
}

/* ************************************************************************** */
/*  COMPOSE RESPONSE                                                          */
/* ************************************************************************** */
std::string Response::compose_response()
{
	if (req.getMethod() == "GET")
	{
		if (req.getFile_extention() == "cgi")
			response = req.getVersion() + full_code(200) + content_type() + content_length(html_code_cgi(req), IS_CGI) + html_code_cgi(req);
		else if (req.getFile() == "/")
		{
			this->response = req.getVersion() + full_code(200) + content_type() + content_length(this->config.get_servers()[server_index]->get_root() + "/" + this->config.get_servers()[server_index]->get_index(), IS_FILE) + body(this->config.get_servers()[server_index]->get_root() + "/" + this->config.get_servers()[server_index]->get_index());
		}
		else if (ft_try_dir(req) != "")
			this->response = req.getVersion() + full_code(200) + content_type() + content_length(ft_try_dir(req), IS_DIR) + ft_try_dir(req);
		else if (exists())
			this->response = req.getVersion() + full_code(200) + content_type() + content_length(this->config.get_servers()[server_index]->get_root() + req.getFile(), IS_FILE) + body(this->config.get_servers()[server_index]->get_root() + req.getFile());
		else
			this->response = req.getVersion() + full_code(200) + content_type() + content_length(this->config.get_servers()[server_index]->get_root() + "/" + this->error_404, IS_FILE) + body(this->config.get_servers()[server_index]->get_root() + "/" + this->error_404);
	}

	else if (req.getMethod() == "POST")
	{
		if (req.getFile_extention() == "cgi")
		{
			std::string a(html_code_cgi(req));
			response = req.getVersion() + full_code(200) + content_type() + content_length(a, IS_CGI) + a;
		}

		else
			this->response = req.getVersion() + full_code(204) + content_type();
	}

	else if (req.getMethod() == "DELETE")
	{
		if (exists())
		{
			remove((this->config.get_servers()[server_index]->get_root() + req.getFile()).c_str());
			this->response = req.getVersion() + full_code(200) + "Content-Type: text/html\nContent-Length: 48\n\n <html><body><h1>File deleted.</h1></body></html>";
		}
		else
			this->response = req.getVersion() + full_code(204);
	}

	else
		this->response = req.getVersion() + full_code(501);
 	return this->response;
}

std::string Response::get_response()
{
	return (compose_response());
}
