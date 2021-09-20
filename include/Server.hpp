#ifndef SATOSHIS_SERVER_HPP
#define SATOSHIS_SERVER_HPP
#include "cpp_libraries.hpp"
#include "Location.hpp"
#include "error.hpp"

class Server
{
private:
	unsigned							listen_port;
	std::list<std::string>				name;
	unsigned							max_body_size;
	std::list<Location> 				locations;
	std::map<unsigned, std::string>		error_page;

public:
	Server() : listen_port(80) {}
	~Server() {}

	// getters
	std::list<Location>	&get_location() { return this->locations; }

	// setters
	void	add_name(std::string n_name)							{ this->name.push_back(n_name); }
	void	add_max_body(unsigned n_max_body)						{ this->max_body_size = n_max_body; }
	void	add_listen_port(unsigned int n_port) 					{ this->listen_port = n_port; }
	void	add_locations(Location n_locations)						{ this->locations.push_back(n_locations); }
	void	add_error_page(std::pair<unsigned, std::string> n_err)	{ this->error_page[n_err.first] = n_err.second; }
};

#endif //SATOSHIS_SERVER_HPP
