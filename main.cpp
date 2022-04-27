#include "webserv.hpp"

int main()
{
	if (select_loop() == -1)
		return (EXIT_FAILURE);
	return (EXIT_SUCCESS);
}
