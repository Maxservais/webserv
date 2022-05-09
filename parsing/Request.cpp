#include "../webserv.hpp"

/* ************************************************************************** */
/*  REQUEST                                                                   */
/* ************************************************************************** */

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/*  CANON                                                                     */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
Request::Request(char *buffer, Config &conf): buff(buffer), config(conf)
{
	if (getMethod() == "POST" && buff.find("------WebKitFormBoundary") != std::string::npos)
	{
		buff.clear();
		for (int i = 0; i < BUFFER_SIZE; i++)
		{
			buff.push_back(buffer[i]);
		}
	}
	std::cout << buff << std::endl;
	this->_file = getFile();
	fill_variables();
	return ;
}

Request::Request(const Request &obj): config(obj.config)
{
	
	this->buff = obj.buff;
	this->_server_index = obj._server_index; 
	this->_max_body_size = obj._max_body_size;
	this->_port = obj._port;
	this->_errors = obj._errors;
	this->_server_name = obj._server_name;
	this->_root = obj._root;
	this->_index = obj._index;
	this->_methods = obj._methods;
	this->_directory_listing = obj._directory_listing;
	this->_uploads = obj._uploads;
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

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/*  UTILS FILLING                                                             */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
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

void Request::fill_variables()
{
	// SET SERVER_INDEX
	this->_server_index = 0;
	std::string tmp = getHost();
	size_t pos = tmp.find(":");
	int port_tmp = atoi(tmp.substr(pos + 1).c_str());

	for(size_t i = 0; i < this->config.get_servers().size(); i++)
	{
		if (this->config.get_servers()[i]->get_port() == port_tmp)
			this->_server_index = i;
	}

	// FIST WE SET EVERYTHING WITH THE GLOBAL SCOPE ELEMENTS
	this->_port = this->config.get_servers()[this->_server_index]->get_port();
	this->_server_name = this->config.get_servers()[this->_server_index]->get_server_name();
	this->_max_body_size = this->config.get_servers()[this->_server_index]->get_max_body_size();
	this->_errors = this->config.get_servers()[this->_server_index]->get_errors();
	this->_root = this->config.get_servers()[this->_server_index]->get_root();
	this->_index = this->config.get_servers()[this->_server_index]->get_index();
	this->_methods = this->config.get_servers()[this->_server_index]->get_methods();
	this->_directory_listing = true;
	this->_uploads = this->_root;

	// NOW CHECK IF THERE IS / IN THE URI
	std::string uri = getFile().erase(0, 1);

	// CASE WE DON'T HAVE A / AND THE FILE NAME IS NOT A LOCATION KEY
	// OR THERE IS NO LOCATION BLOCKS IN THE URI WE KEEP THE GLOBAL SCOPE VARIABLES
	if (this->config.get_servers()[this->_server_index]->get_locations().empty())
		return;

	// CASE WE DON'T HAVE A / AND THE URI IS NOT A LOCATION BLOCK NAME
	if (uri.find('/') == std::string::npos && this->config.get_servers()[this->_server_index]->get_locations().find(getFile()) == this->config.get_servers()[this->_server_index]->get_locations().end())
		return;

	// CASE WE HAVE A / IN THE URI MEANS WE WILL TAKE THE SETTINGS OF THE LOCATION BLOCK (IF THERE IS ONE FOR THIS KEY)
	std::map<std::string,Location *>::iterator it;
	if (uri.find('/') != std::string::npos)
	{
		std::string loc = "/";
		for (size_t i = 0; uri[i] != '/'; i++)
			loc += uri[i];
		it = this->config.get_servers()[this->_server_index]->get_locations().find(loc);
	}
	else
	{
		it = this->config.get_servers()[this->_server_index]->get_locations().find(getFile());
		this->_file = "/";
	}

	// we found the key in the location map
	if (it !=  this->config.get_servers()[this->_server_index]->get_locations().end())
	{
		if (uri.find('/') != std::string::npos)
			this->_file = "/" + uri.substr(it->first.size());

		if ( !it->second->get_root().empty())
			this->_root = it->second->get_root();
		if ( !it->second->get_index().empty())
			this->_index = it->second->get_index();
		if ( !it->second->get_methods().empty())
			this->_methods = it->second->get_methods();
		if ( it->second->get_directory_listing() == false)
			this->_directory_listing = false;
		if ( !it->second->get_uploads().empty())
			this->_uploads = it->second->get_uploads();
		else
			this->_uploads = this->_root;
	}
	return;
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/*  UTILS                                                                     */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

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
		size_t j = a.find("------WebKitFormBoundary", i);
		if (j != std::string::npos)
		{
			return (ft_upload(std::string((a.begin() + i + 3), a.begin() + j - 2), buff));
		}
	}
	return (std::string());
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/*  GETTERS                                                                   */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
int Request::get_server_index(void) const { return this->_server_index; }
std::string Request::get_root(void) const { return this->_root; }
std::string Request::get_index(void) const { return this->_index; }
std::vector<std::string> &Request::get_methods(void) { return this->_methods; }
bool Request::get_directory_listing(void) const { return this->_directory_listing; }
int Request::get_port(void) const { return this->_port; }
std::string Request::get_server_name(void) const { return this->_server_name; }
std::map<int,std::string> &Request::get_errors(void) { return this->_errors; }
std::string Request::get_uploads(void) const { return this->_uploads; }
int  Request::get_max_body_size(void) const { return this->_max_body_size; }
std::string Request::get_file(void) const { return this->_file; }

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


