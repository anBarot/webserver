#include "../include/webserver.hpp"

std::vector<Server> parsing_simulation()
{
	std::vector<Server> servers;
	servers.push_back(Server(8080));
	servers.push_back(Server(8081));
	return servers;
}

void 	add_clients(SocketPool *sp, std::vector<int> lsp, std::vector<Client> clients)
{
	int new_sd;
	for (std::vector<int>::iterator it = lsp.begin(); it != lsp.end(); it++)
	{
		if (FD_ISSET(*it, &(sp->reading_set)))
		{
			new_sd = accept(*it, 0, 0);
			clients.push_back(Client(new_sd));
			return;
		}

		if (FD_ISSET(*it, &(sp->writing_set)))
		{
			new_sd = accept(*it, 0, 0);
			clients.push_back(Client(new_sd));
			return;
		}
	}
}


void 	read_sockets(SocketPool *sp, std::vector<Client> clients)
{
	for (std::vector<Client>::iterator it = clients.begin(); it != clients.end(); it++)
	{
		if (FD_ISSET(it->socket, &(sp->reading_set)))
		{
			// read
			break;
		}
	}
}


void 	write_sockets(SocketPool *sp, std::vector<Client> clients)
{
	for (std::vector<Client>::iterator it = clients.begin(); it != clients.end(); it++)
	{
		if (FD_ISSET(it->socket, &(sp->writing_set)))
		{
			// write
			break;
		}
	}
}


int 	socket_routine(std::vector<int> listen_sockets_pool, std::vector<Client> clients_pool)
{
	SocketPool sp(listen_sockets_pool, clients_pool); // <= ajouter les socket clients dans  le reading set ET writing set
	timeval timeout;

	int waiting_connexions = select(FD_SETSIZE, &sp.reading_set, &sp.writing_set, 0, 0);
	if (waiting_connexions == -1)
		return error_and_exit(SOCK_ERR);
	if (waiting_connexions == 0)
		return (1);
	add_clients(&sp, listen_sockets_pool, clients_pool);
	read_sockets(&sp, clients_pool);
	write_sockets(&sp, clients_pool);
	return 1;
}


int main()
{
	std::vector<int> listen_sockets_pool = listen_sockets_from_servers(parsing_simulation());
	int status = 1;

	std::vector<Client> clients_pool;

	while(status)
	{
		status = socket_routine(listen_sockets_pool, clients_pool);
	}
}