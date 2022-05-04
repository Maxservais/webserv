#include "../webserv.hpp"

void check_Server_blocks(Config &obj)
{
	if (obj.get_servers().empty()) // check if the config file contains at least a server block
	{
		throw EmptyConfErr();
	}

	std::string arr[] = {"GET", "POST", "DELETE", "HEAD", "PUT", "CONNECT", "OPTIONS", "TRACE", "PATCH"}; 
	std::vector<std::string> method_list(arr, arr + sizeof(arr) / sizeof(std::string));
	for (size_t i = 0; i < obj.get_servers().size(); i++) 
	{
		if (!obj.get_servers()[i]->get_port() // check if the server block contains the needed informations
		|| obj.get_servers()[i]->get_server_name().empty()
		|| !obj.get_servers()[i]->get_max_body_size()
		|| obj.get_servers()[i]->get_root().empty()
		|| obj.get_servers()[i]->get_index().empty()
		|| obj.get_servers()[i]->get_methods().empty()
		|| obj.get_servers()[i]->get_errors().empty()
		|| obj.get_servers()[i]->get_locations().empty())
			throw MissStatErr();

		for (size_t j = 0; j < obj.get_servers()[i]->get_methods().size(); j++) // check if the methods are one of the 8 HTTP methods
		{
			std::string tmp = obj.get_servers()[i]->get_methods()[j];
			if (std::find(std::begin(method_list), std::end(method_list), tmp) == std::end(method_list))
				throw MethErr();
		}

		if (obj.get_servers()[i]->get_port() <= 0 || obj.get_servers()[i]->get_max_body_size() <= 0 // check the ports and max_body_size
		|| obj.get_servers()[i]->get_port() > 65535)
			throw NegPortErr();
		
		const char * tmp = (obj.get_servers()[i]->get_root()).c_str(); // check root
		DIR *dir = opendir(tmp);
		if (dir == NULL)
			throw RootErr();
		else
			closedir(dir);

		std::ifstream stream(obj.get_servers()[i]->get_root() + "/" + obj.get_servers()[i]->get_index()); // check index
		if (stream.fail())
			throw IndexErr();
		else
			stream.close();

		if (obj.get_servers()[i]->get_errors().empty()) // check errors pages
			throw MissStatErr();
		for (std::map<int,std::string>::iterator it = obj.get_servers()[i]->get_errors().begin() ;it != obj.get_servers()[i]->get_errors().end(); ++it)
		{
			if (it->first < 100 || it->first > 599)
				throw CodeErr();
			std::ifstream stream(obj.get_servers()[i]->get_root() + "/" + it->second);
			if (stream.fail())
				throw Code_fileErr();
			else
				stream.close();
		}

		if (obj.get_servers()[i]->get_server_name().find('\\') != std::string::npos // check server_name for illegal char
		|| obj.get_servers()[i]->get_server_name().find('/') != std::string::npos
		|| obj.get_servers()[i]->get_server_name().find(':') != std::string::npos
		|| obj.get_servers()[i]->get_server_name().find('?') != std::string::npos
		|| obj.get_servers()[i]->get_server_name().find('"') != std::string::npos
		|| obj.get_servers()[i]->get_server_name().find('>') != std::string::npos
		|| obj.get_servers()[i]->get_server_name().find('|') != std::string::npos
		|| obj.get_servers()[i]->get_server_name().find('<') != std::string::npos)
			throw ServNameErr();

		for (std::map<std::string,Location*>::iterator it = obj.get_servers()[i]->get_locations().begin() ;it != obj.get_servers()[i]->get_locations().end(); ++it)
		{
			const char * tmp = (it->second->get_root()).c_str(); // check root
			DIR *dir = opendir(tmp);
			if (dir == NULL)
				throw RootErr();
			else
				closedir(dir);

			std::ifstream stream(it->second->get_root() + "/" + it->second->get_index()); // check index
			if (stream.fail())
				throw IndexErr();
			else
				stream.close();

			if (!it->second->get_cgi_path().empty())
			{
				std::ifstream stream(it->second->get_root() + "/" + it->second->get_cgi_path()); // check cgi_path
				if (stream.fail())
					throw CgiErr();
				else
					stream.close();
			}

			if (!it->second->get_methods().empty()) // check if the methods are one of the 8 HTTP methods
			{
				for (size_t j = 0; j < it->second->get_methods().size(); j++) 
				{
					std::string tmp = it->second->get_methods()[j];
					if (std::find(std::begin(method_list), std::end(method_list), tmp) == std::end(method_list))
						throw MethErr();
				}
			}
		}
	}
}

// void check_argv(int argc, char **argv)
// {
// 	if (argc != 2)
// 		throw ArgvErr();

// 	std::ifstream conf_file(argv[1]);
// 	if (conf_file.fail())
// 		throw ConfOpenErr();
// 	else
// 		conf_file.close();
// }

void conf_check(int argc, char **argv, Config &config)
{
	if (argc != 2)
		throw ArgvErr();

	std::ifstream conf_file(argv[1]);
	if (conf_file.fail())
		throw ConfOpenErr();
	else
		conf_file.close();

	Config tmp(argv[1]); // SUPER INEFICIENT, LET'S FIX IT LATER
	config = tmp;
	check_Server_blocks(config);
	return;
}
