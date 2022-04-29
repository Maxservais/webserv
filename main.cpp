#include "webserv.hpp"

int main()
{
	int			sockfd;
	sockaddr_in	sockaddr;
	Log			log;

	/* Set-up the signals */
	// if (set_up_signals() != 0)
	// 	return (EXIT_FAILURE);

	/* Set-up the server */
	try
	{
		setup_server(&sockfd, &sockaddr);
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return (EXIT_FAILURE);
	}

	/* Handle clients' requests */
	try
	{
		handle_clients(log, &sockfd, &sockaddr);
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}
