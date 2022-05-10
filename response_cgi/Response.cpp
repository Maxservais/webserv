#include "../webserv.hpp"

/* ************************************************************************** */
/*  CANON                                                                     */
/* ************************************************************************** */
Response::Response(Request &request) : req(request)
{
	// get_server pointeur sur serveur prend port et serveur name
	// vecteur temp -> tous serveurs s'appliquent au port, si un seul
	// -->renvoie celui la, si plusieurs parcours vecteur et compare name si meme renvoie bon, sinon renvoie premier
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
	const std::string &path = req.get_root() + this->req.get_file();
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

std::string Response::check_error_custom(int code)
{
	if (req.get_errors().empty())
		return "";
	std::map<int,std::string>::iterator it = req.get_errors().find(code);
	if (it != req.get_errors().end())
		return (req.get_root() + "/" + it->second);
	else
		return "";
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
	DIR *dir = opendir((this->req.get_root() + request.get_file()).c_str());
	if (dir == NULL)
		return ("");
	ret +=\
	"<!DOCTYPE html>\n<html>\n<head>\n<title>" + dir_name + "</title>\n</head>\n<body>\n<h1>AutoIndex of " + dir_name + "</h1>\n<p>\n";	
	if (dir_name[0] != '/')
		dir_name = "/" + dir_name;
	for (struct dirent *dir_buff = readdir(dir); dir_buff; dir_buff = readdir(dir))
	{
		ret += get_link(std::string(dir_buff->d_name), dir_name, this->req.get_port());
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
/*  RESPONSE COMPOSITION                                                      */
/* ************************************************************************** */
std::string Response::content_type(std::string file)
{
	size_t pos = file.find_last_of(".");
	std::string ext = file.substr(pos + 1);

	if (req.getFile_extention() == "cgi")
		return ("Content-Type: text/html\n");

	if (ext == "html")
		return ("Content-Type: text/html; charset=utf-8\n");

	if (ext == "png" || ext == "jpg" || ext == "ico" || ext == "gif" || ext == "webp")
		return ("Content-Type: image/" + ext + "\n");

	if (req.getMethod() == "POST")
	{
		return ("Content-Type: text/plain\n");
	}
	else
		return ("Content-Type: text/html\n");
}

std::string Response::body(std::string file)
{
	size_t pos = file.find_last_of(".");
	std::string ext = file.substr(pos + 1);
	if (ext == "png" || ext == "jpg" || ext == "ico" || ext == "gif" || ext == "webp")
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
/*  COMPOSE RESPONSE                                                          */
/* ************************************************************************** */
void Response::get_methode()
{
	std::string s;
	if (req.getFile_extention() == "cgi")
	{
		std::string a(html_code_cgi(req));
		this->response = req.getVersion() + full_code(200) + content_type(a) + "Content-Length: " + std::to_string(a.size()) + "\r\n\r\n" + a + "\r\n";
	}
	else if (req.get_file() == "/")
	{
		s = this->req.get_root() + "/" + this->req.get_index();
		this->response = req.getVersion() + full_code(200) + content_type(s) + "Content-Length: " + std::to_string(body(s).size()) + "\r\n\r\n" + body(s) + "\r\n";
	}
	else if (ft_try_dir(req) != "")
		this->response = req.getVersion() + full_code(200) + content_type(ft_try_dir(req)) + "Content-Length: " + std::to_string(ft_try_dir(req).size()) + "\r\n\r\n" + ft_try_dir(req) + "\r\n";
	else if (exists())
	{
		s = this->req.get_root() + req.get_file();
		this->response = req.getVersion() + full_code(200) + content_type(s) + "Content-Length: " + std::to_string(body(s).size()) + "\r\n\r\n" + body(s) + "\r\n";
	}
	else
	{
		std::string tmp = check_error_custom(404);
		if (!tmp.empty())
			this->response = req.getVersion() + full_code(404) + content_type(tmp) + "Content-Length: " + std::to_string(body(tmp).size()) + "\r\n\r\n" + body(tmp) + "\r\n";
		else
			this->response = req.getVersion() + full_code(404) + "Content-Type: text/html\nContent-Length: 99\n\n<html><body><center><h1>Error 404</h1></center><center><h2>Not found<h2></center><hr></body></html>";
	}
}

void Response::post_methode()
{
	if (req.getFile_extention() == "up")
	{
		// if (this->req.get_max_body_size() > """""""")
		// 	response = """"""""
		std::string a(req.getUploadImput());
		std::string b("<h1>File " + a + " has been uploaded successfully</h1>");
		if (a.empty() == 0)
		{
			response = req.getVersion() + full_code(200) + "Content-Type: text/html\nContent-Length: " + std::to_string(b.size()) + "\r\n\r\n" + "<h1>File " + a + " has been uploaded successfully</h1>";
			return ;
		}
	}
	if (req.getFile_extention() == "cgi")
	{
		std::string a(html_code_cgi(req));
		response = req.getVersion() + full_code(200) + content_type(a) + "Content-Length: " + std::to_string(a.size()) + "\r\n\r\n" + a + "\r\n";
	}
	else
	{
		std::string tmp = check_error_custom(204);
		if (!tmp.empty())
			this->response = req.getVersion() + full_code(204) + content_type(tmp) + "Content-Length: " + std::to_string(body(tmp).size()) + "\r\n\r\n" + body(tmp) + "\r\n";
		else
			this->response = req.getVersion() + full_code(204) + "Content-Type: text/html\nContent-Length: 100\n\n<html><body><center><h1>Error 204</h1></center><center><h2>No content<h2></center><hr></body></html>" + "\r\n";
	}
}

void Response::delete_methode()
{
	if (exists())
	{
		remove((this->req.get_root() + req.get_file()).c_str());
		this->response = req.getVersion() + full_code(200) + "Content-Type: text/html\nContent-Length: 48\n\n <html><body><h1>File deleted.</h1></body></html>" + "\r\n";
	}
	else
	{
		std::string tmp = check_error_custom(404);
		if (!tmp.empty())
			this->response = req.getVersion() + full_code(404) + content_type(tmp) + "Content-Length: " + std::to_string(body(tmp).size()) + "\r\n\r\n" + body(tmp) + "\r\n";
		else
		{
			this->response = req.getVersion() + full_code(404) + "Content-Type: text/html\nContent-Length: 99\n\n<html><body><center><h1>Error 404</h1></center><center><h2>Not found<h2></center><hr></body></html>" + "\r\n";
		}
	}
}

std::string Response::compose_response()
{
	std::vector<std::string> v = req.get_methods();
	if (req.getMethod() == "GET" && std::find(v.begin(), v.end(), "GET") != v.end())
		get_methode();

	else if (req.getMethod() == "POST" && std::find(v.begin(), v.end(), "POST") != v.end())
		post_methode();

	else if (req.getMethod() == "DELETE" && std::find(v.begin(), v.end(), "DELETE") != v.end())
		delete_methode();

	else
	{
		std::string tmp = check_error_custom(501);
		if (!tmp.empty())
			this->response = req.getVersion() + full_code(501) + content_type(tmp) + "Content-Length: " + std::to_string(body(tmp).size()) + "\r\n\r\n" + body(tmp) + "\r\n";
		else
			this->response = req.getVersion() + full_code(501) + "Content-Type: text/html\nContent-Length: 105\n\n<html><body><center><h1>Error 501</h1></center><center><h2>Not implemented<h2></center><hr></body></html>" + "\r\n";
	}
	return this->response;
}

std::string Response::get_response()
{
	return (compose_response());
}
