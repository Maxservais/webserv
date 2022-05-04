#include "../webserv.hpp"

// void check_Server_block(Config &obj)
// {
// 	if (obj.get_servers().empty()) // check if the config file contains at least a server block
// 	{
// 		throw EmptyConfErr();
// 	}

// 	for (size_t i = 0; i < obj.get_servers().size(); i++) // check if the server block contains the needed informations
// 	{
// 		if (obj.get_servers()[i]->get_port().empty() 
// 		|| obj.get_servers()[i]->get_server_name().empty()
// 		|| obj.get_servers()[i]->get_max_body_size().empty() 
// 		|| obj.get_servers()[i]->get_root().empty()
// 		|| obj.get_servers()[i]->get_index().empty()
// 		|| obj.get_servers()[i]->get_methods().empty()
// 		|| obj.get_servers()[i]->get_errors().empty()
// 		|| obj.get_servers()[i]->get_locations().empty())
// 			throw MissStatErr();
// 	}

// 	for (size_t i = 0; i < obj.get_servers().size(); i++) // check if the methods are one of the 8 HTTP methods
// 	{
// 		for (size_t j = 0; j < obj.get_servers()[i]->get_methods().size(); j++)
// 		{
// 			std::string tmp = obj.get_servers()[i]->get_methods()[j];
// 			if (!tmp.compare("GET") || !tmp.compare("HEAD") || !tmp.compare("POST") || !tmp.compare("PUT") || !tmp.compare("DELETE") || !tmp.compare("CONNECT") || !tmp.compare("OPTION") || !tmp.compare("TRACE"))
// 				throw MethErr();
// 		}
// 	}
// }

int conf_check(int argc, char **argv)
{
	if (argc != 2)
		throw ArgvErr();

	std::ifstream conf_file(argv[1]);
	if (conf_file.fail())
		throw ConfOpenErr();

	Config test(argv[1]);
	std::cout << "test-->" << test.get_servers()[0]->get_port() << std::endl;
	std::cout << "test-->" << test.get_servers()[1]->get_port() << std::endl;
	// try
	// {
	// 	check_Server_block(test);
	// }
	// catch(const std::exception& e)
	// {
	// 	std::cerr << e.what() << '\n';
	// 	return (EXIT_FAILURE);
	// }
	return (EXIT_SUCCESS);
}
