#include "../include/webserver.hpp"

char buffer[10];
int last_read;

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
			write(1, "added client in reading set\n", strlen("added client in reading set\n"));
			return;
		}

		if (FD_ISSET(*it, &(sp->writing_set)))
		{
			new_sd = accept(*it, 0, 0);
			clients.push_back(Client(new_sd));
			write(1, "added client in writing set\n", strlen("added client in writing set\n"));
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
			last_read = read(it->socket, buffer, 10);
			write(1, "should read something\n", strlen("should read something\n"));
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
			write(1, buffer, last_read);
			break;
		}
	}
}


int 	socket_routine(std::vector<int> listen_sockets_pool, std::vector<Client> clients_pool)
{
	SocketPool sp(listen_sockets_pool, clients_pool); // <= ajouter les socket clients dans  le reading set ET writing set
	timeval timeout;
	timeout.tv_sec = 2;
	timeout.tv_usec = 0;

	int waiting_connexions = select(FD_SETSIZE, &sp.reading_set, &sp.writing_set, 0, &timeout);
	printf("select = %d, FD_SETSIZE = %d\n", waiting_connexions, FD_SETSIZE);
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