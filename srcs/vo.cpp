#include "../include/webserver.hpp"

std::vector<Server> parsing_simulation()
{
	std::vector<Server> servers;
	servers.push_back(Server(8080));
	servers.push_back(Server(8081));
	return servers;
}


int 	read_socket(SocketPool *sp, std::vector<int> lsp, std::vector<Client> clients)
{
	// iteration sur sp.listen_socket jusque trouver un is_set, et l'accepter
	// créer un socket client à partir du socker => Client(socket)
	int new_sd;
	for (std::vector<int>::iterator it = lsp(); it != lsp.end(); it++)
	{
		if (FD_ISSET(*it, sp->reading_set))
		{
			new_sd = accept(*it, 0, 0);
			clients.push_back(Client::Client(new_sd));
			break;
		}
	}
	// re iterer sur les socket clients voir si y'a un truc à lire
}


void 	write_socket(SocketPool *sp)
{
	// itérer sur les clients voir si ils sont dispo en écriture
}


int 	socket_routine(std::vector<int> listen_sockets_pool, std::vector<Client> clients_pool)
{
	SocketPool sp(listen_sockets_pool, clients_pool); // <= ajouter les socket clients dans  le reading set ET writing set

	if (select(FD_SETSIZE, &sp.reading_set, &sp.writing_set, 0, 0) == -1)
		error_and_exit(SOCK_ERR);
	read_sockets(&sp, listen_sockets_pool, clients_pool);
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