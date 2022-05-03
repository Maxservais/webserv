#include "webserv.hpp"

int main(int argc, char **argv)
{
	int			sockfd;
	sockaddr_in	sockaddr;
	Log			log;

	/* Parse the config file */
	if (conf_check(argc, argv))
		return (EXIT_FAILURE);

	/* Set-up the server */
	try
	{
		setup_server(&sockfd, &sockaddr);
	}
	catch (std::exception &e)
	{
		std::cerr << "Program termination. Exception caught!" << std::endl;
		return (EXIT_FAILURE);
	}

	/* Handle clients' requests */
	try
	{
		handle_clients(log, &sockfd, &sockaddr);
	}
	catch (std::exception &e)
	{
		std::cerr << "Program termination. Exception caught!" << std::endl;
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}
