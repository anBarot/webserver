#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#include "cpp_libraries.hpp"
#include "data_struct.hpp"

#include "Server.hpp"
#include "Location.hpp"

# define BACKLOG 5

//parser
int conf_parser(char *file_name);

//tools
void ws_trim(std::string& s);
int error_and_exit(e_error error_nb);
int socket_from_server(Server &server);
int listen_from_server(Server &server);

#endif
