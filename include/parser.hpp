#ifndef PARSER_HPP
#define PARSER_HPP

#include <iostream>
#include <fstream>
#include <list>
#include <vector>
#include <map>

enum methods {
	GET,
	POST,
	DELETE,
	PUT
};

class Location 
{
	private:
		std::string						root;
		std::string						index;
		bool							auto_index;
		std::string						location;
		std::string						upload_path;
		bool							methods[4];
		std::string						cgi_path;
	
	public:
		Location() {
			methods[GET] = false;
			methods[POST] = false;
			methods[DELETE] = false;
			methods[PUT] = false;
		}
		~Location() {}
};

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

#endif
