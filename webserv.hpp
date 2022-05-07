# ifndef WEBSERV_HPP
# define WEBSERV_HPP

/* 0. EXTERNAL LIBRARIES */

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdio>
#include <cctype>
#include <stdexcept>
#include <dirent.h>
#include <signal.h>
#include <map>

/* 1. MACROS AND GLOBAL*/

#define BUFFER_SIZE 8000000

/* 2. CUSTOM CLASSES */

/* 2.1 PARSING OF THE CONFIG FILE */

class Location
{
	public:
		Location(void);
		Location(std::string block);
		Location(Location const & src);
		~Location();
		Location& operator=(Location const & rhs);

		void fill_variables(std::vector<std::string> vec);
		void check_Location(void) const;

		std::string get_ALL(void) const;
		std::string get_root(void) const;
		std::string get_index(void) const;
		std::string get_cgi_extension(void) const;
		std::string get_cgi_path(void) const;
		std::vector<std::string> get_methods(void) const;
		bool get_directory_listing(void) const;

	private:
		std::string _ALL;
		std::string _root;
		std::string _index;
		std::string _cgi_extension;
		std::string _cgi_path;
		std::vector<std::string> _methods;
		bool _directory_listing;
};

class Server
{
	public:
		Server(void);
		Server(std::string block);
		Server(Server const & src);
		~Server();
		Server& operator=(const Server &rhs);

		void fill_variables(std::vector<std::string> vec);
		size_t fill_location(std::vector<std::string> vec, size_t i);

		// void check_Server(void) const;

		std::string get_ALL(void) const;
		int get_port(void) const;
		std::string get_server_name(void) const;
		int get_max_body_size(void) const;
		std::string get_root(void) const;
		std::string get_index(void) const;
		std::vector<std::string> get_methods(void) const;
		std::map<int,std::string> &get_errors(void);
		std::map<std::string, Location*> &get_locations(void);

	private:
		std::string _ALL;
		int _port;
		std::string _server_name;
		int _max_body_size;
		std::string _root;
		std::string _index;
		std::vector<std::string> _methods;
		std::map<int,std::string> _errors;
		std::map<std::string, Location*> _locations;
};

class Config
{
	public:
		Config(void);
		Config(std::string conf_file);
		Config(Config const & src);
		~Config();
		Config& operator=(const Config &rhs);
		std::vector<Server*> get_servers(void) const;
	private:
		std::vector<Server*> _servers;
};

/* 2.2 PARSING OF THE REQUESTS */
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
		std::string getHost();
		std::string getBuff() const;
		std::string getMethod();
		std::string getFile();
		std::string getFile_clean();
		std::string getVersion();
		std::string getFile_extention();
		std::string getQuery();
		std::string getPostImput();
		std::string getUploadImput();
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

class Response
{
	private:
		Request &req;
		Config &config;
		int server_index;
		std::string response;

	public:
		Response(Request &request, Config &config);
		Response	&operator=(const Response &obj);
		~Response();
		bool exists();

		std::string full_code(int code);
		std::string content_type(std::string file);
		std::string body(std::string file);
		std::string compose_response();
		std::string get_response();
		std::string ft_try_dir(Request &request);
		std::string html_code_cgi(Request &req);
		std::string check_error_custom(int code);
		void get_methode();
		void post_methode();
		void delete_methode();
};

// CGI HANDLER
class	Cgi
{
	private:
		char **env;
		Request request;
	public:
		Cgi(Request a);
		Cgi &operator=( Cgi &obj );
		~Cgi();
		void setEnv();
		std::string executeCgi();
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

class ConnectionErr : public std::exception
{
	const char * what () const throw () { return ("Read error occurred while receiving on the socket, closing connection"); }
};

class TimeOutErr : public std::exception
{
	const char * what () const throw () { return ("Time out, closing connection"); }
};

class ArgvErr : public std::exception
{
	const char * what () const throw () { return ("./webserv [path to configuration file]"); }
};

class ConfOpenErr : public std::exception
{
	const char * what () const throw () { return ("Cannot open configuration file"); }
};

class ExtErr : public std::exception
{
	const char * what () const throw () { return ("Wrong extension for the configuration file"); }
};

class EmptyConfErr : public std::exception
{
	const char * what () const throw () { return ("Invalid configuration file"); }
};

class MissStatErr : public std::exception
{
	const char * what () const throw () { return ("Missing element in the configuration file"); }
};

class MethErr : public std::exception
{
	const char * what () const throw () { return ("Invalid methods in the configuration file"); }
};

class NegPortErr : public std::exception
{
	const char * what () const throw () { return ("Invalid port number or max body_size"); }
};

class RootErr : public std::exception
{
	const char * what () const throw () { return ("Invalid root"); }
};

class IndexErr : public std::exception
{
	const char * what () const throw () { return ("Cannot open the index file requested"); }
};

class CodeErr : public std::exception
{
	const char * what () const throw () { return ("Wrong error code"); }
};

class Code_fileErr : public std::exception
{
	const char * what () const throw () { return ("Cannot open the error code file"); }
};

class ServNameErr : public std::exception
{
	const char * what () const throw () { return ("Wrong formating of the server_name"); }
};

class CgiErr : public std::exception
{
	const char * what () const throw () { return ("Cannot open cgi script"); }
};

/* 4. MAIN FUNCTIONS */

/* 4.0 PARSER_DISPATCHER_TMP */
std::string read_file_to_str(const std::string& path);
bool exists (Request request);
std::string get_length_file(std::string file);
std::string convert_to_binary(const char * path);
std::string dispatcher(Request &request);
Config &conf_check(int argc, char **argv, Config &config);

/* 4.1 SETUP SERVER */
void	close_sockets(int *sockets, int len);
void	setup_server(int *sockets, Config &config, std::vector<struct sockaddr_in> &sockaddr);

/* 4.2 HANDLE CLIENTS */
void	handle_clients(int *sockets, Config &config, Log log, std::vector<struct sockaddr_in> &sockaddr);

#endif