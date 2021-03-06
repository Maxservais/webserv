#include "webserv.hpp"

int main(int argc, char **argv)
{
	Config		config;

	/* Parse the config file */
	try
	{
		config = conf_check(argc, argv, config);
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		std::cerr << "Program termination. Issue(s) with config file!" << std::endl;
		config.free_all_server();
		return (EXIT_FAILURE);
	}

	/* Initialize some variables to handle later listening on multiple ports */
	int	len = config.get_nb_port();

	std::vector<struct sockaddr_in>	sockaddr(len);
	std::vector<int> sockets(len);

	/* Fill in sockaddr structs (1 per socket) */
	for (int i = 0; i < len; ++i)
	{
		sockaddr[i].sin_family = PF_INET;
		sockaddr[i].sin_port = htons(config.get_ports_vector()[i]);
		sockaddr[i].sin_addr.s_addr = INADDR_ANY;
		memset(sockaddr[i].sin_zero, '\0', sizeof sockaddr[i].sin_zero);
	}

	/* Set-up the server */
	try
	{
		setup_server(sockets, config, sockaddr);
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		config.free_all_server();
		return (EXIT_FAILURE);
	}

	/* Handle clients' requests */
	try
	{
		handle_clients(sockets, config, sockaddr);
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		config.free_all_server();
		return (EXIT_FAILURE);
	}

	config.free_all_server();
	return (EXIT_SUCCESS);
}
