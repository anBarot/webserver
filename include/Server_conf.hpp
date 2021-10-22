#ifndef SATOSHIS_SERVER_HPP
#define SATOSHIS_SERVER_HPP

#include "cpp_libraries.hpp"
#include "Location.hpp"

class Server_conf
{
	public:
		unsigned							listen_port;
		std::list<std::string>				names;
		unsigned							max_body_size;
		std::map<std::string, Location> 	locations;
		std::map<unsigned, std::string>		error_page;
	
		Server_conf() : listen_port(80), max_body_size(1) 
		{
			locations[""] = Location();
		}
		Server_conf(int port) : listen_port(port) {}
		~Server_conf() {}
};

#endif //SATOSHIS_SERVER_HPP
