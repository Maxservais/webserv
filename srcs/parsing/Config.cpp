#include "../../webserv.hpp"

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

Location::Location(std::string block) : _ALL(block), _directory_listing(true)
{
	std::string token;
	std::vector<std::string> vec;
	size_t pos = 0;
	while((pos = this->_ALL.find("\t")) != std::string::npos )
	{
		token = this->_ALL.substr(0, pos);
		if (!token.empty())
			vec.push_back(token);
		this->_ALL.erase(0,pos + 1);
	}
	if (!this->_ALL.empty() && this->_ALL[0] != '}')
		vec.push_back(this->_ALL);

	fill_variables(vec);
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

/* Filler */
void Location::fill_variables(std::vector<std::string> vec)
{
	size_t i = 0;
	while (i < vec.size())
	{
		if (vec[i].find("root") != std::string::npos)
			this->_root.assign(vec[i].begin() + vec[i].find(" ") + 1, vec[i].end());
		else if (vec[i].find("index") != std::string::npos)
			this->_index.assign(vec[i].begin() + vec[i].find(" ") + 1, vec[i].end());
		else if (vec[i].find("uploads") != std::string::npos)
			this->_uploads.assign(vec[i].begin() + vec[i].find(" ") + 1, vec[i].end());
		else if (vec[i].find("method") != std::string::npos)
		{
			this->_methods = split_spaces(vec[i]);
			this->_methods.erase(this->_methods.begin());
		}
		else if (vec[i].find("directory_listing ") != std::string::npos)
		{
			if (vec[i].find("off") != std::string::npos)
				this->_directory_listing = false;
			if (vec[i].find("on") != std::string::npos)
				this->_directory_listing = true;
		}	
		i++;
	}
}

/* Getters */
std::string Location::get_ALL(void) const {return this->_ALL; }
std::string Location::get_root(void) const { return this->_root; }
std::string Location::get_uploads(void) const { return this->_uploads; }
std::string Location::get_index(void) const { return this->_index; }
std::vector<std::string> Location::get_methods(void) const { return this->_methods; }
bool Location::get_directory_listing(void) const { return this->_directory_listing; }

/* ************************************************************************** */
/*  SERVER                                                                    */
/* ************************************************************************** */

/* Canon */
Server::Server() { return; }

Server::Server(std::string block) : _ALL(block), _port(0), _max_body_size(0)
{
	std::string token;
	std::vector<std::string> vec;
	size_t pos = 0;
	while((pos = this->_ALL.find("\n")) != std::string::npos )
	{
		token = this->_ALL.substr(0, pos);
		if (!token.empty())
			vec.push_back(token);
		this->_ALL.erase(0,pos + 2); // the + 2 is for removing the tabs
	}
	fill_variables(vec);
}

Server::Server(Server const & src)
{
	*this = src;
	return;
}

Server& Server::operator=(const Server &rhs)
{
	this->_ALL = rhs._ALL;
	this->_port = rhs._port;
	this->_server_name = rhs._server_name;
	this->_max_body_size = rhs._max_body_size;
	this->_root = rhs._root;
	this->_index = rhs._index;
	this->_methods = rhs._methods;
	this->_errors = rhs._errors;
	this->_locations = rhs._locations;
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

/* Filler */
size_t Server::fill_location(std::vector<std::string> vec, size_t i)
{
	std::string token;
	std::string tmp;
	Location * loc_tmp;

	while(vec[i] != "}")
	{
		token += vec[i];
		i++;
	}
	loc_tmp = new Location(token);
	tmp.clear();
	tmp.assign(token.begin() + token.find("/"), token.begin() + token.find("{"));
	tmp.pop_back();
	this->_locations.insert(std::pair<std::string, Location*>(tmp,loc_tmp));
	return i;
}

void Server::fill_variables(std::vector<std::string> vec)
{
	size_t i = 0;
	std::string tmp;

	while (i < vec.size())
	{
		if (vec[i].find("listen ") != std::string::npos)
		{
			tmp.assign(vec[i].begin() + vec[i].find(" ") + 1, vec[i].end());
			this->_port = atoi(tmp.c_str());
		}
		else if (vec[i].find("server_name ") != std::string::npos)
			this->_server_name.assign(vec[i].begin() + vec[i].find(" ") + 1, vec[i].end());
		else if (vec[i].find("max_body_size ") != std::string::npos)
		{
			tmp.clear();
			tmp.assign(vec[i].begin() + vec[i].find(" ") + 1, vec[i].end());
			this->_max_body_size = atoi(tmp.c_str());
		}
		else if (vec[i].find("root ") != std::string::npos)
			this->_root.assign(vec[i].begin() + vec[i].find(" ") + 1, vec[i].end());
		else if (vec[i].find("index ") != std::string::npos)
			this->_index.assign(vec[i].begin() + vec[i].find(" ") + 1, vec[i].end());
		else if (vec[i].find("error ") != std::string::npos)
		{
			std::vector<std::string> splited = split_spaces(vec[i]);
			this->_errors.insert(std::pair<int, std::string>(atoi(splited[1].c_str()), splited[2]));
		}
		else if (vec[i].find("method ") != std::string::npos)
		{
			this->_methods = split_spaces(vec[i]);
			this->_methods.erase(this->_methods.begin());
		}
		else if (vec[i].find("location ") != std::string::npos)
			i = fill_location(vec, i);
		i++;
	}
}

/* Getters */
std::string Server::get_ALL(void) const {return this->_ALL; }
int Server::get_port(void) const { return this->_port; }
std::string Server::get_server_name(void) const { return this->_server_name; }
int Server::get_max_body_size(void) const { return this->_max_body_size; }
std::string Server::get_root(void) const { return this->_root; }
std::string Server::get_index(void) const { return this->_index; }
std::vector<std::string> Server::get_methods(void) const { return this->_methods; }
std::map<int,std::string> &Server::get_errors(void){ return this->_errors;}
std::map<std::string, Location*> &Server::get_locations(void) { return this->_locations; }

/* ************************************************************************** */
/*  CONFIG                                                                    */
/* ************************************************************************** */

/* Canon */
Config::Config() { return; }

Config::Config(std::string conf_file)
{
	std::ifstream input(conf_file);
	std::string ALL = std::string((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
	size_t pos = 0;
	std::string token;
	Server * tmp;
	if (ALL.find("server {") == std::string::npos)
		return;
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
	input.close();
	set_nb_port();
}

Config::Config(Config const & src)
{
	this->_servers = src._servers;
	return;
}

Config::~Config() { return; }

Config& Config::operator=(const Config &rhs)
{
	if (this != &rhs)
		this->_servers = rhs.get_servers();
	return (*this);
}

/* Getters */
std::vector<Server*> Config::get_servers(void) const { return this->_servers; }
int Config::get_nb_port(void) const { return this->_nb_port; }

/* Utils */
void Config::set_nb_port(void)
{
	std::map<int,int> tmp;
	for(int i = 0; i < static_cast<int>(this->get_servers().size()); i++)
		tmp.insert(std::pair<int, int>(this->get_servers()[i]->get_port(), 0));
	this->_nb_port = tmp.size();
}

void Config::free_all_server(void)
{
	std::vector<Server*>::iterator ite = this->_servers.end();
	for (std::vector<Server*>::iterator it = this->_servers.begin(); it != ite; ++it)
		delete (*it);
	this->_servers.clear();
	return;
}
