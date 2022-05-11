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
#include <map>
#include <cstdio>
#include <cctype>
#include <stdexcept>
#include <dirent.h>
#include <signal.h>
#include <sys/stat.h>

/* 1. MACROS*/

#define BUFFER_SIZE 8000000

/* 2. CUSTOM CLASSES */

/* 2.1 PARSING OF THE CONFIG FILE */
class Location
{
	public:
		/* Canon */
		Location(void);
		Location(std::string block);
		Location(Location const & src);
		~Location();
		Location& operator=(Location const & rhs);

		/* Utils */
		void fill_variables(std::vector<std::string> vec);
		void check_Location(void) const;

		/* Getters */
		std::string get_ALL(void) const;
		std::string get_root(void) const;
		std::string get_index(void) const;
		std::string get_uploads(void) const;
		std::vector<std::string> get_methods(void) const;
		bool get_directory_listing(void) const;

	private:
		std::string _ALL;
		std::string _root;
		std::string _index;
		std::vector<std::string> _methods;
		bool _directory_listing;
		std::string _uploads;
};

class Server
{
	public:
		/* Canon */
		Server(void);
		Server(std::string block);
		Server(Server const & src);
		~Server();
		Server& operator=(const Server &rhs);

		/* Utils */
		void fill_variables(std::vector<std::string> vec);
		size_t fill_location(std::vector<std::string> vec, size_t i);

		/* Getters */
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
		void free_all_server(void);

	private:
		std::vector<Server*> _servers;
};

/* 2.2 PARSING OF THE REQUESTS */
class	Request
{
	public:
		/* Canon */
		Request(char *buffer, Config &conf);
		Request(const Request &obj);
		Request &operator=(const Request &obj);
		~Request();

		/* Utils */
		std::string getBuff() const;
		std::string getMethod();
		std::string setFile();
		std::string getFile_clean();
		std::string getVersion();
		std::string getFile_extention();
		std::string getQuery();
		std::string getPostImput();
		std::string getUploadImput();
		int getUpBody();
		std::vector<std::string> split_words(std::string buffer);
		std::string ft_upload(std::string up, std::string buff);

		/* Utils to fill the variables to be used in response */
		std::string getHost();
		void fill_default_variables();
		void fill_server_index();
		void fill_variables();
		void replace_default_variables(std::map<std::string,Location *>::iterator it, std::string uri);

		/* Getters */
		std::string get_file(void) const;
		int  get_server_index(void) const;
		std::string  get_root(void) const;
		std::string  get_errors_root(void) const;
		std::string  get_index(void) const;
		std::vector<std::string>  &get_methods(void);
		bool  get_directory_listing(void) const;
		int  get_port(void) const;
		std::string  get_server_name(void) const;
		std::map<int,std::string>  &get_errors(void);
		std::string  get_uploads(void) const;
		int  get_max_body_size(void) const;

	private:
		std::string buff;
		Config &config;
		std::string _file;

		int _server_index; 
		int _max_body_size;
		int _port;
		std::map<int,std::string> _errors;
		std::string _server_name;
		std::string _root;
		std::string _errors_root;
		std::string _index;
		std::vector<std::string> _methods;
		bool _directory_listing;
		std::string _uploads;
};

class	Log
{
	public:
		/* Canon */
		Log();
		Log( const Log &obj );
		Log &operator=( const Log &obj );
		~Log();

		/* Getters */
		Request getFirst() const;
		Request getLast() const;
		std::vector<Request> getLog() const;

		/* Utils */
		void	add_one(Request newone);
		void	clear();
		size_t	size() const;

	private:
		std::vector<Request> v;
};

/* 2.3 FORMULATION OF THE RESPONSE */
class Response
{
	public:
		/* Canon */
		Response(Request &request);
		Response	&operator=(const Response &obj);
		~Response();
		bool exists();

		/* Utils */
		std::string full_code(int code);
		std::string content_type(std::string file);
		std::string body(std::string file);
		std::string compose_response();
		std::string get_response();
		std::string ft_try_dir(Request &request);
		std::string html_code_cgi(Request &req);
		std::string check_error_custom(int code);
		std::string compose_error_message(int code);

		/* Getters */
		void get_methode();
		void post_methode();
		void delete_methode();

	private:
		Request &req;
		std::string response;
};

/* 2.4 CGI HANDLER*/
class	Cgi
{
	public:
		Cgi(Request a);
		Cgi &operator=( Cgi &obj );
		~Cgi();
		void setEnv();
		std::string executeCgi();

	private:
		char **env;
		Request request;
};

/* 3. EXCEPTIONS */

/* 3.1 EXCEPTIONS CONFIG FILE CHECK */
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

class OpenErr : public std::exception
{
	const char * what () const throw () { return ("Can't upload the requested file"); }
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

/* 3.2 SERVER */
class SocketErr : public std::exception
{
	const char * what () const throw () { return ("Failed to create socket!"); }
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

/* 3.3 CGI */
class CgiErr : public std::exception
{
	const char * what () const throw () { return ("Cannot open cgi script"); }
};

/* 4. MAIN FUNCTIONS */

/* 4.0 PARSING AND RESPONSE */
bool exists (Request request);
Config &conf_check(int argc, char **argv, Config &config);

/* 4.1 SETUP SERVER */
void	close_sockets(int *sockets, int len);
void	setup_server(int *sockets, Config &config, std::vector<struct sockaddr_in> &sockaddr);

/* 4.2 HANDLE CLIENTS */
void	handle_clients(int *sockets, Config &config, Log log, std::vector<struct sockaddr_in> &sockaddr);

#endif