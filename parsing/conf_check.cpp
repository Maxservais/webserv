#include "../webserv.hpp"

void conf_check(int argc, char **argv)
{
	if (argc != 2)
		throw ArgvErr();

	std::ifstream conf_file(argv[1]);
	if (conf_file.fail())
		throw ConfOpenErr();

	Config test(argv[1]);
}
