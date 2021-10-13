#include "../include/webserver.hpp"

int main(int ac, char **av)
{
	std::vector<Server_conf> servers_conf;
	std::vector<Client> clients_pool;
	std::vector<int> listen_sockets_pool;
	int status = 1;

	if (ac > 2)
		return (error_and_exit(ARG_ERR));

	if (conf_parser(av[1], servers_conf))
		return (error_and_exit(CONFFILE_PARSE_ERR));

	display_server(servers_conf);

	listen_sockets_pool = listen_sockets_from_servers(servers_conf);

	while(status)
	{
		status = socket_routine(listen_sockets_pool, clients_pool);
	}

	return (0);
}
