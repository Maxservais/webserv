#include "Config.hpp"

Location::Location() { return; }

Location::Location(std::string block) : _ALL(block) { return; }

Location::Location(Location const & src)
{
	*this = src;
	return;
}

Location::~Location() { return; }

std::string Location::get_ALL(void) const {return this->_ALL; }

//

Server::Server() { return; }

Server::Server(std::string block) : _ALL(block) { return; }

Server::Server(Server const & src)
{
	*this = src;
	return;
}

Server::~Server() { return; }

std::string Server::get_ALL(void) const {return this->_ALL; }

std::map<std::string, Location> Server::get_locations(void) const { return this->_locations; }

//

Config::Config() { return; }

Config::Config(std::string conf_file)
{
	std::ifstream input(conf_file);
	std::string ALL;
	if (!input.is_open())
		std::cout << "Failed to open the requested ressource" << std::endl;
	else
		ALL = std::string((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
	
	size_t pos = 0;
	std::string token;
	while((pos = ALL.find("server {")) != std::string::npos )
	{
		token = ALL.substr(0, pos);
		if (!token.empty())
			this->_servers.push_back(token);
		ALL.erase(0,pos + 8);
	}
	return;
}

Config::Config(Config const & src)
{
	*this = src;
	return;
}

Config::~Config() { return; }

std::vector<Server> Config::get_servers(void) const { return this->_servers; }