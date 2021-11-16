#include "../include/webserver.hpp"

std::map<int, std::string> reason_phrase;
std::map<std::string, std::string> MIME_types;
pthread_mutex_t error_file_mutex; 
pthread_mutex_t index_file_mutex; 

void sigint_handler(int signum)
{
	std::cout << "\nQuitting webserver\n";
	exit(signum);
}

int main(int ac, char **av)
{
	std::vector<Server_conf> servers_conf;
	std::vector<Client> clients_pool;
	std::map<int, int> listen_sockets_pool;
	int status = 1;

	if (ac > 2)
		return (error_and_exit(ARG_ERR));

	if (conf_parser(av[1], servers_conf))
		return (error_and_exit(CONFFILE_PARSE_ERR));

	// display_servers(servers_conf);
	maps_init_MIME_types(MIME_types);
	maps_init_reason_phrase(reason_phrase);
	pthread_mutex_init(&error_file_mutex, NULL);
	pthread_mutex_init(&index_file_mutex, NULL);
	listen_sockets_pool = listen_sockets_from_servers(servers_conf);

	signal(SIGINT, sigint_handler);

	while(status)
		status = socket_routine(listen_sockets_pool, clients_pool, servers_conf);

	return (0);
}
