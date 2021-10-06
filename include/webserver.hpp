#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#include "cpp_libraries.hpp"
#include "data_struct.hpp"
#include "Server.hpp"
#include "Location.hpp"
#include "Client.hpp"
#include "SocketPool.hpp"

//parser
int conf_parser(char *file_name, std::vector<Server_conf> &servers);

//tools
void ws_trim(std::string& s);
int error_and_exit(e_error error_nb);
int	is_method(std::string str);
void strlower(std::string &str);
int socket_from_server(Server_conf &server);
int listen_from_server(Server_conf &server);
void display_server(std::vector<Server_conf> &servers);

//sockets
std::vector<int> listen_sockets_from_servers(std::vector<Server_conf> servers);
int socket_routine(std::vector<int> listen_sockets_pool, std::vector<Client> clients_pool);


#endif
