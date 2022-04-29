#include "webserv.hpp"

int ft_error(std::string message)
{
	std::cout << message << std::endl;
	exit(EXIT_FAILURE);
}

// void shutdown()
// {
	
// }

// void handler(int sig_num)
// {
// 	if (sig_num == SIGINT)
// 	{
// 		std::cout << "SIGINT !" << std::endl;
// 		shutdown();
// 	}
// }

// int set_up_signals()
// {
// 	struct sigaction signals;
// 	signals.handler = handler;
// 	if (sigaction(SIGINT, &signals, 0) == -1)
// 	{
// 		std::cout << "Sigaction issue" << std::endl;
// 		return -1;
// 	}
// 	return 0;
// }
