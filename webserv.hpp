# ifndef WEBSERV_HPP
# define WEBSERV_HPP

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <vector>
#include <cstdio>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <cctype>
#include <sstream>

#define SERVER_PORT 9999
#define BUFFER_SIZE 1000000
#define MAX_CONNECTIONS 10

//PARSING
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

//PARSER_DISPATCHER_TMP
std::string read_file_to_str(const std::string& path);
bool exists (Request request);
std::string get_length_file(std::string file);
std::string convert_to_binary(const char * path);
std::string dispatcher(Request &request);

//SERVER_SETUP
int setup_server();
int select_loop(Log log);

//UTILS
int ft_error(std::string message);

#endif