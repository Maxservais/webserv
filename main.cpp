#include "webserv.hpp"

int main(int argc, char **argv)
{
	int			sockfd;
	int			sockfd1;
	sockaddr_in	sockaddr;
	sockaddr_in	sockaddr1;
	int			*sockets = (int *)malloc(100 * sizeof(int));
	Config		config;
	Log			log;

	/* Parse the config file */
	try
	{
		conf_check(argc, argv, &config);
	}
	catch (std::exception &e)
	{
		std::cerr << "Program termination. Issue(s) with config file!" << std::endl;
		return (EXIT_FAILURE);
	}

	/* Set-up the server */
	try
	{
		setup_server(sockets, &config, &sockfd, &sockfd1, &sockaddr, &sockaddr1);
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		// std::cerr << "Program termination. Exception caught!" << std::endl;
		return (EXIT_FAILURE);
	}

	/* Handle clients' requests */
	try
	{
		handle_clients(sockets, &config, log, &sockfd, &sockaddr, &sockfd1, &sockaddr1);
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		// std::cerr << "Program termination. Exception caught!" << std::endl;
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}
