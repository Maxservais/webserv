#include "../webserv.hpp"

/* ************************************************************************** */
/*  CHECK SERVER BLOCKS                                                       */
/* ************************************************************************** */

void Scheck_info(Config &obj, int i) // check if the server block contains the needed informations
{
	if (!obj.get_servers()[i]->get_port() 
		|| obj.get_servers()[i]->get_server_name().empty()
		|| !obj.get_servers()[i]->get_max_body_size()
		|| obj.get_servers()[i]->get_root().empty()
		|| obj.get_servers()[i]->get_index().empty()
		|| obj.get_servers()[i]->get_methods().empty()
		|| obj.get_servers()[i]->get_errors().empty()
		|| obj.get_servers()[i]->get_locations().empty())
			throw MissStatErr();
}

void Scheck_methods(Config &obj, int i) // check if the methods are one of the 8 HTTP methods
{
	std::string arr[] = {"GET", "POST", "DELETE", "HEAD", "PUT", "CONNECT", "OPTIONS", "TRACE", "PATCH"};
	std::vector<std::string> method_list(arr, arr + sizeof(arr) / sizeof(std::string));
	for (size_t j = 0; j < obj.get_servers()[i]->get_methods().size(); j++) 
	{
		std::string tmp = obj.get_servers()[i]->get_methods()[j];
		if (std::find(std::begin(method_list), std::end(method_list), tmp) == std::end(method_list))
			throw MethErr();
	}
}

void Scheck_root_index(Config &obj, int i) // check root and index
{
	const char * tmp = (obj.get_servers()[i]->get_root()).c_str(); 
	DIR *dir = opendir(tmp);
	if (dir == NULL)
		throw RootErr();
	else
		closedir(dir);

	std::ifstream stream(obj.get_servers()[i]->get_root() + "/" + obj.get_servers()[i]->get_index()); 
	if (stream.fail())
		throw IndexErr();
	else
		stream.close();
}

void Scheck_port_body_size(Config &obj, int i) // check the ports and max_body_size
{
	if (obj.get_servers()[i]->get_port() <= 0 || obj.get_servers()[i]->get_max_body_size() <= 0 
	|| obj.get_servers()[i]->get_port() > 65535)
		throw NegPortErr();
}

void Scheck_errors(Config &obj, int i) // check errors pages
{
	if (obj.get_servers()[i]->get_errors().empty()) 
			throw MissStatErr();
	for (std::map<int,std::string>::iterator it = obj.get_servers()[i]->get_errors().begin(); it != obj.get_servers()[i]->get_errors().end(); ++it)
	{
		if (it->first < 100 || it->first > 599)
			throw CodeErr();
		std::ifstream stream(obj.get_servers()[i]->get_root() + "/" + it->second);
		if (stream.fail())
			throw Code_fileErr();
		else
			stream.close();
	}
}

void Scheck_server_name(Config &obj, int i) // illegal chars in server_name
{
	if (obj.get_servers()[i]->get_server_name().find('\\') != std::string::npos
	|| obj.get_servers()[i]->get_server_name().find('/') != std::string::npos
	|| obj.get_servers()[i]->get_server_name().find(':') != std::string::npos
	|| obj.get_servers()[i]->get_server_name().find('?') != std::string::npos
	|| obj.get_servers()[i]->get_server_name().find('"') != std::string::npos
	|| obj.get_servers()[i]->get_server_name().find('>') != std::string::npos
	|| obj.get_servers()[i]->get_server_name().find('|') != std::string::npos
	|| obj.get_servers()[i]->get_server_name().find('<') != std::string::npos)
			throw ServNameErr();
}

/* ************************************************************************** */
/*  CHECK LOCATION BLOCKS                                                     */
/* ************************************************************************** */

void Lcheck_root_index(std::map<std::string,Location*>::iterator it) // check index and index
{
	const char * tmp = (it->second->get_root()).c_str();
	DIR *dir = opendir(tmp);
	if (dir == NULL)
		throw RootErr();
	else
		closedir(dir);

	std::ifstream stream(it->second->get_root() + "/" + it->second->get_index()); 
	if (stream.fail())
		throw IndexErr();
	else
		stream.close();
}

void Lcheck_cgi(std::map<std::string,Location*>::iterator it) // check cgi_path
{
	if (!it->second->get_cgi_path().empty())
	{
		std::ifstream stream(it->second->get_root() + "/" + it->second->get_cgi_path()); 
		if (stream.fail())
			throw CgiErr();
		else
			stream.close();
	}
}

void Lcheck_methods(std::map<std::string,Location*>::iterator it) // check if the methods are one of the 8 HTTP methods
{
	std::string arr[] = {"GET", "POST", "DELETE", "HEAD", "PUT", "CONNECT", "OPTIONS", "TRACE", "PATCH"};
	std::vector<std::string> method_list(arr, arr + sizeof(arr) / sizeof(std::string));
	if (!it->second->get_methods().empty()) 
	{
		for (size_t j = 0; j < it->second->get_methods().size(); j++) 
		{
			std::string tmp = it->second->get_methods()[j];
			if (std::find(std::begin(method_list), std::end(method_list), tmp) == std::end(method_list))
				throw MethErr();
		}
	}
}

/* ************************************************************************** */
/*  GLOBAL CHECKS                                                             */
/* ************************************************************************** */
void check_Server_blocks(Config &obj) 
{
	if (obj.get_servers().empty()) 
		throw EmptyConfErr();

	for (size_t i = 0; i < obj.get_servers().size(); i++) 
	{
		Scheck_info(obj, i);
		Scheck_methods(obj, i);
		Scheck_root_index(obj, i);
		Scheck_port_body_size(obj, i);
		Scheck_errors(obj, i);
		Scheck_server_name(obj, i);

		for (std::map<std::string,Location*>::iterator it = obj.get_servers()[i]->get_locations().begin() ;it != obj.get_servers()[i]->get_locations().end(); ++it)
		{
			Lcheck_root_index(it);
			Lcheck_cgi(it);
			Lcheck_methods(it);
		}
	}
}

void conf_check(int argc, char **argv, Config &config)
{
	if (argc != 2) // check two args
		throw ArgvErr();

	std::string str = argv[1]; // check config fiel extension
	int position= str.find_last_of(".");
	if (str.substr(position + 1) != "conf")
		throw ExtErr();

	std::ifstream conf_file(argv[1]); // check can open config file
	if (conf_file.fail())
		throw ConfOpenErr();
	else
		conf_file.close();


	Config tmp(argv[1]); // SUPER INEFFICIENT, LET'S FIX IT LATER
	config = tmp;
	check_Server_blocks(config);
	return;
}
