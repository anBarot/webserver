#include "../include/webserver.hpp"

char buffer[BUFFER_SIZE];
int last_read;
char response[8000] = "HTTP/1.1 200 OK\r\n\n";



std::vector<Server> parsing_simulation()
{
	std::vector<Server> servers;
	servers.push_back(Server(8080));
	servers.push_back(Server(8081));
	return servers;
}


void 	add_clients(SocketPool *sp, std::vector<int> lsp, std::vector<Client> *clients)
{
	int new_sd;
	for (std::vector<int>::iterator it = lsp.begin(); it != lsp.end(); it++)
	{
		if (FD_ISSET(*it, &(sp->reading_set)))
		{
			new_sd = accept(*it, 0, 0);
			(*clients).push_back(Client(new_sd));
			write(1, "added client in reading set\n", strlen("added client in reading set\n"));
			return;
		}

		if (FD_ISSET(*it, &(sp->writing_set)))
		{
			new_sd = accept(*it, 0, 0);
			(*clients).push_back(Client(new_sd));
			write(1, "added client in writing set\n", strlen("added client in writing set\n"));
			return;
		}
	}
}


void 	read_sockets(SocketPool *sp, std::vector<Client> *clients)
{
	int DEBUG_c = 0;
	int DEBUG_rd = 0;
	for (std::vector<Client>::iterator it = (*clients).begin(); it != (*clients).end(); it++)
	{
		DEBUG_c++;
		if (FD_ISSET(it->socket, &(sp->reading_set)))
		{
			DEBUG_rd++;
			last_read = read(it->socket, buffer, BUFFER_SIZE);
			it->store_incoming_data(buffer, last_read);
			write(1, buffer, last_read);
			break;
		}
	}
	printf("%d clients - %d read\n", DEBUG_c, DEBUG_rd);
}


void 	write_sockets(SocketPool *sp, std::vector<Client> *clients)
{
	int DEBUG_c = 0;
	int DEBUG_wt = 0;
	for (std::vector<Client>::iterator it = (*clients).begin(); it != (*clients).end(); it++)
	{
		DEBUG_c++;
		if (FD_ISSET(it->socket, &(sp->writing_set)) && !it->requests.empty()) // si le client a des requetes à traiter
		{
			DEBUG_wt++;
			write(it->socket, response, strlen(response));
			if (it->requests.empty()) // si il n'y a plus de requetes pour le client, on le nique
				clients->erase(it);
			break;
		}
	}
	printf("%d clients, %d write\n", DEBUG_c, DEBUG_wt);
}


int 	socket_routine(std::vector<int> listen_sockets_pool, std::vector<Client> *clients_pool)
{
	SocketPool sp(listen_sockets_pool, *clients_pool); // <= ajouter les socket clients dans  le reading set ET writing set
	timeval timeout;
	timeout.tv_sec = 2;
	timeout.tv_usec = 0;

	int waiting_connexions = select(FD_SETSIZE, &sp.reading_set, &sp.writing_set, 0, &timeout);
	printf("select = %d\n", waiting_connexions);
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
		status = socket_routine(listen_sockets_pool, &clients_pool);
	}
}