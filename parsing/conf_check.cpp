#include "../webserv.hpp"

void check_Server_block(Config &obj)
{
	if (obj.get_servers().empty()) // check if the config file contains at least a server block
	{
		throw EmptyConfErr();
	}

	for (size_t i = 0; i < obj.get_servers().size(); i++) // check if the server block contains the needed informations
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

	std::string arr[] = {"GET", "POST", "DELETE", "HEAD", "PUT", "CONNECT", "OPTIONS", "TRACE", "PATCH"}; // check if the methods are one of the 8 HTTP methods
	std::vector<std::string> method_list(arr, arr + sizeof(arr) / sizeof(std::string));
	for (size_t i = 0; i < obj.get_servers().size(); i++) 
	{
		for (size_t j = 0; j < obj.get_servers()[i]->get_methods().size(); j++)
		{
			std::string tmp = obj.get_servers()[i]->get_methods()[j];
			if (std::find(std::begin(method_list), std::end(method_list), tmp) == std::end(method_list))
				throw MethErr();
		}
	}

	for (size_t i = 0; i < obj.get_servers().size(); i++) // check the ports and max_body_size
	{
		if (obj.get_servers()[i]->get_port() <= 0 || obj.get_servers()[i]->get_max_body_size() <= 0
		|| obj.get_servers()[i]->get_port() > 65535)
			throw NegPortErr();
	}
}

void check_argv(int argc, char **argv)
{
	if (argc != 2)
		throw ArgvErr();

	std::ifstream conf_file(argv[1]);
	if (conf_file.fail())
		throw ConfOpenErr();
}

int conf_check(int argc, char **argv)
{
	try
	{
		check_argv(argc, argv);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return (EXIT_FAILURE);
	}

	Config test(argv[1]);
	try
	{
		check_Server_block(test);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}
