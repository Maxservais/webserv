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

Server::Server(std::string block) : _ALL(block)
{
	// std::cout << "TEST--> " << this->_ALL << std::endl;
	// std::cout << "~~~~~~~~~NEW SERVER BLOCK~~~~~~~~~" << std::endl;
	std::string tmp = this->_ALL;
	std::string token;
	std::vector<std::string> vec_tmp;
	size_t pos = 0;
	while((pos = tmp.find("\n")) != std::string::npos )
	{
		token = tmp.substr(0, pos);
		if (!token.empty())
			vec_tmp.push_back(token);
		tmp.erase(0,pos + 2); // the + 2 is for removing the tabs
	}
	// std::vector<std::string>::size_type sz = vec_tmp.size();
	// for (unsigned i=0; i<sz; i++)
	// 	std::cout << vec_tmp[i] << std::endl;

	for(size_t i = 0; i < vec_tmp.size(); i++)
	{
		if (vec_tmp[i].find("listen") != std::string::npos)
			this->_port.assign(vec_tmp[i].begin() + vec_tmp[i].find(" ") + 1, vec_tmp[i].end() - 1);
		else if (vec_tmp[i].find("server_name") != std::string::npos)
			this->_server_name.assign(vec_tmp[i].begin() + vec_tmp[i].find(" ") + 1, vec_tmp[i].end() - 1);
		else if (vec_tmp[i].find("max_body_size") != std::string::npos)
			this->_max_body_size.assign(vec_tmp[i].begin() + vec_tmp[i].find(" ") + 1, vec_tmp[i].end() - 1);
		else if (vec_tmp[i].find("root") != std::string::npos)
			this->_root.assign(vec_tmp[i].begin() + vec_tmp[i].find(" ") + 1, vec_tmp[i].end() - 1);
		else if (vec_tmp[i].find("index") != std::string::npos)
			this->_index.assign(vec_tmp[i].begin() + vec_tmp[i].find(" ") + 1, vec_tmp[i].end() - 1);
		else if (vec_tmp[i].find("error") != std::string::npos)
			
	}

	// std::cout << this->_port << std::endl;
	// std::cout << this->_server_name << std::endl;
	// std::cout << this->_max_body_size << std::endl;
	// std::cout << this->_root << std::endl;
	// std::cout << this->_index << std::endl;
}

Server::Server(Server const & src)
{
	*this = src;
	return;
}

Server::~Server() { return; }

std::string Server::get_ALL(void) const {return this->_ALL; }

std::map<std::string, Location*> Server::get_locations(void) const { return this->_locations; }

//

Config::Config() { return; }

Config::Config(std::string conf_file)
{
	std::ifstream input(conf_file);
	std::string ALL;
	if (!input.is_open())
		std::cout << "Failed to open the requested ressource" << std::endl; // try catch needed here
	else
		ALL = std::string((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
	
	size_t pos = 0;
	std::string token;
	Server * tmp;
	while((pos = ALL.find("server {")) != std::string::npos )
	{
		token = ALL.substr(0, pos);
		if (!token.empty())
		{
			tmp = new Server(token);
			// std::cout << "TOKEN -->" << tmp.get_ALL() << std::endl;
			this->_servers.push_back(tmp);
		}
		ALL.erase(0,pos + 8);
	}
	tmp = new Server(ALL);
	this->_servers.push_back(tmp);
	// std::cout << "TOKEN -->" << tmp_bis.get_ALL() << std::endl;
	return;
}

Config::Config(Config const & src)
{
	*this = src;
	return;
}

Config::~Config()
{
	std::vector<Server*>::iterator ite = this->_servers.end();
	for (std::vector<Server*>::iterator it = this->_servers.begin(); it != ite; ++it)
		delete *it;
	this->_servers.clear();
	return;
}

Config& Config::operator=(const Config &obj)
{
	(void) obj;
	return (*this);
}

std::vector<Server*> Config::get_servers(void) const { return this->_servers; }