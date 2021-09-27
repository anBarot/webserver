#ifndef SATOSHIS_SERVER_HPP
#define SATOSHIS_SERVER_HPP
#include "cpp_libraries.hpp"
#include "Location.hpp"

class Server_conf
{
	private:
		unsigned							listen_port;
		std::list<std::string>				names;
		unsigned							max_body_size;
		std::list<Location> 				locations;
		std::map<unsigned, std::string>		error_page;
	
	public:
		Server_conf() : listen_port(80), max_body_size(1) {}
		Server_conf(int port) : listen_port(port) {}
		~Server_conf() {}
	
		// getters
		std::list<Location>					&get_location() 			{ return this->locations; }
		unsigned 							get_listen_port()			{ return this->listen_port; }
		std::list<std::string>				&get_name() 				{ return this->names; }
		unsigned							get_max_body_size() 		{ return this->max_body_size; }
		std::string							get_error_page(unsigned i)	{ return this->error_page.at(i); }	
	
		// setters
		void	add_name(std::string n_name)							{ this->names.push_back(n_name); }
		void	add_name_front(std::string n_name)						{ this->names.push_front(n_name); }
		void	add_max_body(unsigned n_max_body)						{ this->max_body_size = n_max_body; }
		void	add_listen_port(unsigned int n_port) 					{ this->listen_port = n_port; }
		void	add_locations(Location n_locations)						{ this->locations.push_back(n_locations); }
		void	add_error_page(unsigned err_num, std::string err_file)	{ this->error_page[err_num] = err_file; }
};

#endif //SATOSHIS_SERVER_HPP
