#include "webserv.hpp"

Response::Response(Request &request, std::string path, std::string default_page, std::string error_404) : req(request), path(path), default_page(default_page), error_404(error_404)
{
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
			this->default_page.erase(0, this->default_page.rfind("."));
			this->default_page.erase(0, 1);
		}
		else
			extension = req.getFile_extention();

		if (extension == "html")
			return ("Content-Type: text/html; charset=utf-8\n");
		else if (extension == "png" || extension == "jpg" || extension == "ico")
			return ("Content-Type: image/png; Content-Transfer-Encoding: binary\n");
	}
	else if (req.getMethod() == "POST" || (req.getMethod() == "DELETE" && !exists()))
		return ("Content-Type: text/plain\n");
	return ("Content-Type: text/html; charset=utf-8\n");
	
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
		std::ifstream input_file(path);
		if (!input_file.is_open())
		{
			std::cout << "Failed to open the requested ressource" << std::endl;
			exit(EXIT_FAILURE);
		}
		return std::string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
	}
}

std::string Response::compose_response()
{
	if (req.getMethod() == "GET")
	{
		if (req.getFile() == "/")
			this->response = req.getVersion() + full_code(200) + content_type() + content_length(this->path + this->default_page) + body(this->path + this->default_page);
		else if (exists())
			this->response = req.getVersion() + full_code(200) + content_type() + content_length(this->path + req.getFile()) + body(this->path + req.getFile());
		else
			this->response = req.getVersion() + full_code(200) + content_type() + content_length(this->path + this->error_404) + body(this->path + this->error_404);
	}
	else if (req.getMethod() == "POST")
		this->response = req.getVersion() + full_code(204) + content_type();
	else if (req.getMethod() == "DELETE")
	{
		if (exists())
		{
			remove((this->path + req.getFile()).c_str());
			this->response = req.getVersion() + full_code(200) + content_type() + content_length(this->path + "file.html") + body(this->path + "file.html");
		}
		else
			this->response = req.getVersion() + full_code(204) + content_type();
		
	}
	else // not a post delete or get
		this->response = req.getVersion() + full_code(204) + content_type();
	return this->response;
}

std::string Response::get_response()
{
	return (compose_response());
}
