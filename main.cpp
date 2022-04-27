#include "webserv.hpp"

int main()
{
	Log log;
	if (select_loop(log) == -1)
		return (EXIT_FAILURE);
	return (EXIT_SUCCESS);
}
