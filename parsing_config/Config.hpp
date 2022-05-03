# ifndef CONFIG_HPP
# define CONFIG_HPP

#include <vector>
#include <map>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <fstream>

class Location
{
	public:
		Location(void);
		Location(std::string block);
		Location(Location const & src);
		~Location();
		// Location* operator=(Location const & rhs);

		std::string get_ALL(void) const;

	private:
		std::string _ALL;
		// std::string _root;
		// std::vector<std::string> _methods;
		// std::string _index;
		// bool directory_listing;
		// std::string _cgi_extension;
		// std::string _cgi_path;
};

class Server
{
	public:
		Server(void);
		Server(std::string block);
		Server(Server const & src);
		~Server();
		// Server* operator=(Server const & rhs);

		std::string get_ALL(void) const;
		std::map<std::string, Location*> get_locations(void) const;

	private:
		std::string _ALL;
		// std::string _port;
		// std::string _server_name;

		// std::string _max_body_size;
		// std::vector<std::string> _methods;
		// std::string _root;
		// std::string _index;
		// std::map<int,std::string> _errors;
		std::map<std::string, Location*> _locations;
};

class Config
{
	public:
		Config(void);
		Config(std::string conf_file);
		Config(Config const & src);
		~Config();
		// Config* operator=(Config const & rhs);
		std::vector<Server*> get_servers(void) const;
	private:
		std::vector<Server*> _servers;
};

#endif