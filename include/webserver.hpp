#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#include "cpp_libraries.hpp"
#include "data_struct.hpp"
#include "Server_conf.hpp"
#include "Location.hpp"
#include "Client.hpp"
#include "SocketPool.hpp"

// Tools
void maps_init();
void ws_trim(std::string& s);
void strlower(std::string &str);
int error_and_exit(e_error error_nb);
e_methods get_method_enum(std::string word);
void display_server(std::vector<Server_conf> &servers);

// Headers
std::string get_date(time_t now);
std::string get_MIME(std::string filename);
std::string	get_file_size(std::string file_name);

// Parser
int conf_parser(char *file_name, std::vector<Server_conf> &servers);

// Sockets
int socket_from_server(Server_conf &server);
int listen_from_server(Server_conf &server);
std::vector<int> listen_sockets_from_servers(std::vector<Server_conf> servers);
int socket_routine(std::vector<int> listen_sockets_pool, std::vector<Client> clients_pool);
void send_response(Client &client, int csock);

#endif
