#include "Config.hpp"

/* ************************************************************************** */
/*  UTILS                                                                     */
/* ************************************************************************** */

std::vector<std::string> split_spaces(std::string s)
{
	std::stringstream ss(s);
	std::istream_iterator<std::string> begin(ss);
	std::istream_iterator<std::string> end;
	return (std::vector<std::string>(begin, end));
}

/* ************************************************************************** */
/*  LOCATION                                                                  */
/* ************************************************************************** */

/* Canon */
Location::Location() { return; }

Location::Location(std::string block) : _ALL(block)
{
	std::cout << "~~~~~~~~~NEW LOCATION BLOCK~~~~~~~~~" << std::endl;
	std::string tmp = this->_ALL;
	std::string token;
	std::vector<std::string> vec_tmp;
	size_t pos = 0;
	while((pos = tmp.find("\t")) != std::string::npos )
	{
		token = tmp.substr(0, pos);
		if (!token.empty())
			vec_tmp.push_back(token);
		tmp.erase(0,pos + 1); // the + 2 is for removing the tabs
	}

	std::vector<std::string>::iterator terator;
	for(terator = vec_tmp.begin(); terator != vec_tmp.end(); ++terator)
		std::cout << "loc : " << (*terator) << std::endl;
	
	// size_t i = 0;
	// while (i < vec_tmp.size())
	// {
	// 	if (vec_tmp[i].find("root") != std::string::npos)
	// 		this->_root.assign(vec_tmp[i].begin() + vec_tmp[i].find(" ") + 1, vec_tmp[i].end() - 1);
	// 	else if (vec_tmp[i].find("index") != std::string::npos)
	// 		this->_index.assign(vec_tmp[i].begin() + vec_tmp[i].find(" ") + 1, vec_tmp[i].end() - 1);
	// 	else if (vec_tmp[i].find("cgi_extension") != std::string::npos)
	// 		this->_cgi_extension.assign(vec_tmp[i].begin() + vec_tmp[i].find(" ") + 1, vec_tmp[i].end() - 1);
	// 	else if (vec_tmp[i].find("index") != std::string::npos)
	// 		this->_cgi_path.assign(vec_tmp[i].begin() + vec_tmp[i].find(" ") + 1, vec_tmp[i].end() - 1);
	// 	else if (vec_tmp[i].find("method") != std::string::npos)
	// 	{
	// 		this->_methods = split_spaces(vec_tmp[i]);
	// 		this->_methods.erase(this->_methods.begin());
	// 		this->_methods[this->_methods.size() - 1].pop_back(); // removes the ;
	// 	}
	// 	else if (vec_tmp[i].find("directory_listing") != std::string::npos)
	// 	{
	// 		if (vec_tmp[i].find("on"))
	// 			this->_directory_listing = true;
	// 		else
	// 			this->_directory_listing = false;
	// 	}
	// 	i++;
	// }
	return;
}

Location::Location(Location const & src)
{
	*this = src;
	return;
}

Location::~Location() { return; }

Location& Location::operator=(const Location &rhs)
{
	(void) rhs;
	return (*this);
}

/* Getters */
std::string Location::get_ALL(void) const {return this->_ALL; }
std::string Location::get_root(void) const { return this->_root; }
std::string Location::get_index(void) const { return this->_index; }
std::string Location::get_cgi_extension(void) const { return this->_cgi_extension; }
std::string Location::get_cgi_path(void) const { return this->_cgi_path; }
std::vector<std::string> Location::get_methods(void) const { return this->_methods; }
bool Location::get_directory_listing(void) const { return this->_directory_listing; }

/* ************************************************************************** */
/*  SERVER                                                                    */
/* ************************************************************************** */

/* Canon */
Server::Server() { return; }

Server::Server(std::string block) : _ALL(block)
{
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

	size_t i = 0;
	while (i < vec_tmp.size())
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
		{
			std::vector<std::string> splited = split_spaces(vec_tmp[i]);
			splited[2].pop_back();
			this->_errors.insert(std::pair<int, std::string>(std::stoi(splited[1]), splited[2])); // stoi cpp11
		}
		else if (vec_tmp[i].find("method") != std::string::npos)
		{
			this->_methods = split_spaces(vec_tmp[i]);
			this->_methods.erase(this->_methods.begin());
			this->_methods[this->_methods.size() - 1].pop_back(); // removes the ;
		}
		else if (vec_tmp[i].find("location") != std::string::npos)
		{
			Location * loc_tmp;
			token.clear();
			while(vec_tmp[i] != "}")
			{
				token += vec_tmp[i];
				i++;
			}
			loc_tmp = new Location(token);
			tmp.clear();
			tmp.assign(token.begin() + token.find("/"), token.begin() + token.find("{"));
			this->_locations.insert(std::pair<std::string, Location*>(tmp,loc_tmp));
		}
		i++;
	}

	// std::cout << "ERRORS" << std::endl;
	// std::map<int, std::string>::iterator it;
	// for(it = this->_errors.begin(); it != this->_errors.end(); ++it)
	// 	std::cout << "test for loop : " << (*it).first << " - " << (*it).second << std::endl;

	// std::cout << "\nLOCATIONS" << std::endl;
	// std::map<std::string, Location*>::iterator bla;
	// for(bla = this->_locations.begin(); bla != this->_locations.end(); ++bla)
	// 	std::cout << "test for loop : " << (*bla).first << " - " << (*bla).second->get_ALL() << std::endl;

	// std::cout << "\nMETHODS" << std::endl;
	// std::vector<std::string>::iterator terator;
	// for(terator = this->_methods.begin(); terator != this->_methods.end(); ++terator)
	// 	std::cout << "methods : " << (*terator) << std::endl;

	// std::cout << "\nREST" << std::endl;
	// std::cout << this->_port << std::endl;
	// std::cout << this->_server_name << std::endl;
	// std::cout << this->_max_body_size << std::endl;
	// std::cout << this->_root << std::endl;
	// std::cout << this->_index << std::endl;
	// std::cout << "\n" << std::endl;
}

Server::Server(Server const & src)
{
	*this = src;
	return;
}

Server& Server::operator=(const Server &rhs)
{
	(void) rhs;
	return (*this);
}

Server::~Server()
{
	std::map<std::string, Location*>::iterator ite = this->_locations.end();
	for (std::map<std::string, Location*>::iterator it = this->_locations.begin(); it != ite; ++it)
		delete (*it).second;
	this->_locations.clear();
	return;
}

/* Getters */
std::string Server::get_ALL(void) const {return this->_ALL; }
std::string Server::get_port(void) const { return this->_port; }
std::string Server::get_server_name(void) const { return this->_server_name; }
std::string Server::get_max_body_size(void) const { return this->_max_body_size; }
std::string Server::get_root(void) const { return this->_root; }
std::string Server::get_index(void) const { return this->_index; }
std::vector<std::string> Server::get_methods(void) const { return this->_methods; }
std::map<int,std::string> Server::get_errors(void) const { return this->_errors; }
std::map<std::string, Location*> Server::get_locations(void) const { return this->_locations; }

/* ************************************************************************** */
/*  CONFIG                                                                    */
/* ************************************************************************** */

/* Canon */
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
			this->_servers.push_back(tmp);
		}
		ALL.erase(0,pos + 8);
	}
	tmp = new Server(ALL);
	this->_servers.push_back(tmp);
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

Config& Config::operator=(const Config &rhs)
{
	(void) rhs;
	return (*this);
}

/* Getters */
std::vector<Server*> Config::get_servers(void) const { return this->_servers; }