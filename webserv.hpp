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

#define SERVER_PORT 9999
#define BUFFER_SIZE 1000000
#define MAX_CONNECTIONS 10

//PARSER_DISPATCHER_TMP
std::string read_file_to_str(const std::string& path);
std::string convert_to_binary(const char * path);
std::vector<std::string> parser(char * buffer);
std::string dispatcher(std::vector<std::string> vector);

//SERVER_SETUP
int setup_server();
int select_loop();

//UTILS
int ft_error(std::string message);

#endif