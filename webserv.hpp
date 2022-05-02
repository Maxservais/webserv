# ifndef WEBSERV_HPP
# define WEBSERV_HPP

/* 0. EXTERNAL LIBRARIES */

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h> // do we need this?
#include <stdio.h> // do we need this?
#include <string.h> // can we use <string> instead?
#include <errno.h> // do we need this?
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdio>
#include <cctype>
#include <stdexcept>
#include <dirent.h>

/* 1. MACROS */

#define SERVER_PORT 9999
#define BACKLOG 800
#define BUFFER_SIZE 1000000
#define MAX_CONNECTIONS 10
#define IS_FILE 0
#define IS_DIR 1

/* 1.1. CGI ENVIRONNEMENT TABLE */

const std::string env[24] =
{
	"COMSPEC", "DOCUMENT_ROOT", "GATEWAY_INTERFACE", "HTTP_ACCEPT",
	"HTTP_ACCEPT_ENCODING", "HTTP_ACCEPT_LANGUAGE", "HTTP_CONNECTION",
	"HTTP_HOST", "HTTP_USER_AGENT", "PATH", "QUERY_STRING", "REMOTE_ADDR",
	"REMOTE_PORT", "REQUEST_METHOD", "REQUEST_URI", "SCRIPT_FILENAME",
	"SCRIPT_NAME", "SERVER_ADDR", "SERVER_ADMIN", "SERVER_NAME", "SERVER_PORT",
	"SERVER_PROTOCOL", "SERVER_SIGNTURE", "SERVER_SORTWARE"
};

/* 2. CUSTOM CLASSES */

class	Request
{
	private:
		std::string buff;
	public:
		Request();
		Request(std::string &buffer);
		Request(char *buffer);
		Request(const Request &obj);
		Request &operator=(const Request &objz);
		~Request();
		std::string getBuff() const;
		std::string getMethod();
		std::string getFile();
		std::string getFile_clean();
		std::string getVersion();
		std::string getFile_extention();
		std::vector<std::string> split_words(std::string buffer);
};

class	Log
{
	private:
		std::vector<Request> v;
	public:
		Log();
		Log( const Log &obj );
		Log &operator=( const Log &obj );
		~Log();
		Request getFirst() const;
		Request getLast() const;
		std::vector<Request> getLog() const;
		void	add_one(Request newone);
		void	clear();
		size_t	size() const;
};

//RESPONSE COMPOSER
class Response
{
	private:
		Request &req;
		std::string path;
		std::string default_page;
		std::string error_404;
		std::string response;
		int port;
	public:
		Response(Request &request, std::string path, std::string default_page, std::string error_404, int port);
		Response	&operator=(const Response &obj);
		~Response();
		bool exists();
		std::string full_code(int code);
		std::string content_length(std::string file, int hint);
		std::string content_type();
		std::string body(std::string file);
		std::string compose_response();
		std::string get_response();
		std::string ft_try_dir(Request &request);
};

/* 3. EXCEPTIONS */

class SocketErr : public std::exception
{
	const char * what () const throw ()
	{
		return ("Failed to create socket!");
	}
};

class BindErr : public std::exception
{
	const char * what () const throw () { return ("Failed to bind!"); }
};

class SetsockErr : public std::exception
{
	const char * what () const throw () { return ("setsockopt!"); }
};

class ListenErr : public std::exception
{
	const char * what () const throw () { return ("Failed to listen on socket!"); }
};

class SelectErr : public std::exception
{
	const char * what () const throw () { return ("Failed to select!"); }
};

class AcceptErr : public std::exception
{
	const char * what () const throw () { return ("Failed to grab connection!"); }
};

class SendErr : public std::exception
{
	const char * what () const throw () { return ("Failed to send!"); }
};


/* 4. MAIN FUNCTIONS */

/* 4.0 PARSER_DISPATCHER_TMP */
std::string read_file_to_str(const std::string& path);
bool exists (Request request);
std::string get_length_file(std::string file);
std::string convert_to_binary(const char * path);
std::string dispatcher(Request &request);

/* 4.1 SETUP SERVER */
void	setup_server(int *sockfd, struct sockaddr_in *sockaddr);

/* 4.2 HANDLE CLIENTS */
void	handle_clients(Log log, int *sockfd, struct sockaddr_in *sockaddr);

/* 4.3 UTILS */
int ft_error(std::string message);

#endif