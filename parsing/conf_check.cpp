#include "../webserv.hpp"

void check_Server_block(Config obj)
{
	if (obj.get_servers().empty())
	{
		throw EmptyConfErr();
	}
	for (size_t i = 0; i < obj.get_servers().size(); i++)
	{
		if (obj.get_servers()[i]->get_port().empty() 
		|| obj.get_servers()[i]->get_server_name().empty()
		|| obj.get_servers()[i]->get_max_body_size().empty() 
		|| obj.get_servers()[i]->get_root().empty()
		|| obj.get_servers()[i]->get_index().empty()
		|| obj.get_servers()[i]->get_methods().empty()
		|| obj.get_servers()[i]->get_errors().empty()
		|| obj.get_servers()[i]->get_locations().empty())
			throw MissStatErr();
	}
}

int conf_check(int argc, char **argv)
{
	if (argc != 2)
		throw ArgvErr();

	std::ifstream conf_file(argv[1]);
	if (conf_file.fail())
		throw ConfOpenErr();

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
