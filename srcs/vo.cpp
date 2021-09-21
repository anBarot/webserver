#include "../include/webserver.hpp"

std::vector<Server> parsing_simulation()
{
	std::vector<Server> servers;
	servers.push_back(Server(8080));
	servers.push_back(Server(8081));
	return servers;
}

void 	read_socket(SocketPool *sp)
{


	// iteration sur sp.listen_socket jusque trouver un is_set, et l'accepter
	// créer un client à partir du socket
	// le foutre dans un write set ??


}

void 	write_socket(SocketPool *sp)
{
	// demander à marie qu'elle me prenne par la main

}

int 	socket_routine(std::vector<int> listen_sockets_pool)
{
	SocketPool sp(listen_sockets_pool);

	if (select(FD_SETSIZE, &sp.reading_set, 0, 0, 0) == -1)
		error_and_exit(SOCK_ERR);
	read_sockets(&sp);
	write_sockets(&sp);
}

int main()
{
	std::vector<int> listen_sockets_pool = listen_sockets_from_servers(parsing_simulation());
	int status = 1;


	std::stack<Client> clients_pool; // quelle portée / duree de vie pour les clients

	while(status)
	{
		status = socket_routine(listen_sockets_pool);
	}
}