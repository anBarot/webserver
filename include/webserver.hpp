#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#include "cpp_libraries.hpp"
#include "data_struct.hpp"
#include "Server.hpp"
#include "Location.hpp"
#include "Client.hpp"
#include "SocketPool.hpp"

# define BACKLOG 32

//parser
int conf_parser(char *file_name);

//tools
void ws_trim(std::string& s);
int error_and_exit(e_error error_nb);
int socket_from_server(Server_conf &server);
int listen_from_server(Server_conf &server);

//sockets
std::vector<int> listen_sockets_from_servers(std::vector<Server_conf> servers);

#endif
