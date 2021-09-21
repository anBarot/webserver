#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#include "cpp_libraries.hpp"
#include "data_struct.hpp"

#include "Server.hpp"
#include "Location.hpp"

#include "Client.hpp"

# define BACKLOG 32

//parser
int conf_parser(char *file_name);

//tools
void ws_trim(std::string& s);
int error_and_exit(e_error error_nb);
bool	is_value_in_listen_sockets(int fd, std::vector<int> ls);


//sockets
std::vector<int> listen_sockets_from_servers(std::vector<Server> servers);
#endif
