#include "../include/webserver.hpp"

int main(int ac, char **av)
{
	std::vector<Server_conf> servers;

	if (ac > 2)
		return (error_and_exit(ARG_ERR));

	if (conf_parser(av[1], servers))
		return (error_and_exit(CONFFILE_PARSE_ERR));

	display_server(servers);

	std::vector<int> listen_sockets_pool = listen_sockets_from_servers(servers);

	int status = 1;

	std::vector<Client> clients_pool;

	while(status)
	{
		status = socket_routine(listen_sockets_pool, clients_pool);
	}

	return (0);
}
