#include "../webserv.hpp"

/* ************************************************************************** */
/*  REQUEST                                                                   */
/* ************************************************************************** */

Request::Request()
{
	return ;
}

Request::Request(std::string &buffer): buff(buffer)
{
	return ;
}

Request::Request(char *buffer): buff(buffer)
{
	// if (getMethod() == "POST" && buff.find("------WebKitFormBoundary") != std::string::npos)
	if (getMethod() == "POST" && buff.find("----WebKitFormBoundary") != std::string::npos)
	{
		buff.clear();
		for (int i = 0; i < BUFFER_SIZE; i++)
		{
			buff.push_back(buffer[i]);
		}
	}
	// std::cout << buff << std::endl;
	return ;
}

Request::Request(const Request &obj)
{
	*this = obj;
}

Request	&Request::operator=(const Request &obj)
{
	buff = obj.buff;
	return (*this);
}

Request::~Request()
{
	return ;
}

// split a string with spaces without care of allocations and returns it into a vector
std::vector<std::string> Request::split_words(std::string s)
{
	std::stringstream ss(s);
	std::istream_iterator<std::string> begin(ss);
	std::istream_iterator<std::string> end;
	return (std::vector<std::string>(begin, end));
}

//return buff in private
std::string Request::getBuff() const
{
	return (buff);
}
//return a std::string of the method in capital letters
//if the method doesn't exist, it returns a clear std:::string
std::string Request::getMethod()
{
	std::vector<std::string> v = split_words(buff);
	if (v.empty() == 1)
		return (std::string());
	if (v.at(0) == "GET" || v.at(0) == "POST" || v.at(0) == "DELETE")
		return (v.at(0));
	return (std::string());
}

// return the path of the file, if it exists
// else, it return only a "/"

std::string Request::getFile()
{
	std::vector<std::string> v = split_words(buff);
	if (v.size() < 2)
		return (std::string("/"));
	std::string a = v.at(1);
	if (a.find("?") != std::string::npos)
	{
		a.erase((a.begin() + a.find("?")), a.end());
	}
	return (a);
}

std::string Request::getQuery()
{
	std::vector<std::string> v = split_words(buff);
	if (v.size() < 2)
		return (std::string("/"));
	std::string a = v.at(1);
	if (a.find("?") != std::string::npos)
	{
		a.erase(a.begin(), (a.begin() + a.find("?") + 1));
	}
	else
	{
		return (std::string());
	}
	return (a);
}

//return the HTTP version of the request (ex : HTTP/1.1)
// in case of error, it returns an empty string ("")
std::string Request::getVersion()
{
	std::vector<std::string> v = split_words(buff);
	if (v.size() < 3)
		return (std::string());
	return (v.at(2));
}

// returns the file without the "/", if it exists
// return an empty string in case of error
std::string Request::getFile_clean()
{
	std::string str = getFile();
	if (str.find("/") == 0)
		str.erase(0, 1);
	return (str);
}

// returns the extension of the file
// returns an empty string in case of error
std::string Request::getFile_extention()
{
	std::string str = getFile();
	str.erase(0, str.rfind("."));
	str.erase(0, 1);
	return (str);
}

std::string Request::getPostImput()
{
	if (getMethod() != "POST")
		return (std::string());
	std::string a(buff);
	size_t i = a.rfind("=");
	if (i != std::string::npos)
	{
		size_t j = a.rfind("\n", i);
		if (j != std::string::npos)
			return (std::string((a.begin() + j + 1), a.end()));
	}
	return (std::string());
}

std::string ft_upload(std::string up, std::string buff)
{
	std::string a(buff);
	size_t i = a.rfind("filename=\"");
	if (i != std::string::npos)
	{
		i += 10;
		size_t j = a.find("\"", i);
		if (j != std::string::npos)
		{
			a = std::string((a.begin() + i), a.begin() + j);
		}
	}
	int fd = open(("ressources/download/" + a).c_str(), O_RDWR | O_CREAT, 00777);
	if (fd == -1)
		exit (1); // throw une erreur --> plus tard
	write(fd, up.c_str(), up.size());
	close(fd);
	return (a);
}

std::string Request::getUploadImput()
{
	if (getMethod() != "POST")
		return (std::string());
	std::string a(buff);
	size_t i = a.rfind("Content-Type:");
	if (i != std::string::npos)
		i = a.find("\n", i);
	if (i != std::string::npos)
	{
		// size_t j = a.find("------WebKitFormBoundary", i);
		size_t j = a.find("----WebKitFormBoundary", i);
		if (j != std::string::npos)
		{
			return (ft_upload(std::string((a.begin() + i + 3), a.begin() + j - 2), buff));
		}
	}
	return (std::string());
}
std::string Request::getHost()
{
	std::vector<std::string> v = split_words(buff);
	for(size_t i = 0; i < v.size(); i++)
	{
		if (v[i] == "Host:" && v.size() >= i+1)
			return v[i+1];
	}
	return "";
}


/* ************************************************************************** */
/*  LOG                                                                       */
/* ************************************************************************** */
Log::Log(): v()
{
	return;
}

Log::Log( const Log &obj ): v(obj.v)
{
	return ;
}

Log	&Log::operator=( const Log &obj )
{
	v = obj.v;
	return (*this);
}

Log::~Log()
{
	return ;
}

// return a vector of the history of requests sort from the oldest to the newest
std::vector<Request> Log::getLog() const
{
	return (v);
}

//return the oldest request
Request Log::getFirst() const
{
	return (*v.begin());
}

//return the last request done
Request Log::getLast() const
{
	return (*(v.end() - 1));
}

//add a request to the history
void	Log::add_one(Request newone)
{
	v.push_back(newone);
}

// clear the history
void	Log::clear()
{
	v.clear();
}

// returns the number of requests in the log
size_t	Log::size() const
{
	return (v.size());
}


