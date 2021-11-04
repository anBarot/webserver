#include "../include/webserver.hpp"

/*
 * On regarde les socket listeners
 * on ajoute un new client si un des fd est prêt en reading
 * demander à marie de vérif la logique du merdier
 */
void 	add_clients(SocketPool &sp, std::map<int, int> lsp, std::vector<Client> &clients)
{
	int new_sd;
	for (std::map<int, int>::iterator it = lsp.begin(); it != lsp.end(); it++)
	{
		if (FD_ISSET(it->first, &(sp.reading_set)))
		{
			new_sd = accept(it->first, 0, 0);
			clients.push_back(Client(new_sd, it->second));
			write(1, "added client in reading set\n", strlen("added client in reading set\n"));
			return;
		}

		if (FD_ISSET(it->first, &(sp.writing_set)))
		{
			new_sd = accept(it->first, 0, 0);
			clients.push_back(Client(new_sd, it->first));
			write(1, "added client in writing set\n", strlen("added client in writing set\n"));
			return;
		}
	}
}

/*
 *  On parcourt les socket clients : si y'a qch à lire, on store la data lue dans l'objet Client
 * 	Si y'a rien à lire, on destroy l'objet Client
 */
void 	read_from_clients_sockets(SocketPool &sp, std::vector<Client> &clients)
{
	char buffer[BUFFER_SIZE];
	int last_read;
	int DEBUG_c = 0;
	int DEBUG_rd = 0;

	for (std::vector<Client>::iterator it = clients.begin(); it != clients.end(); it++)
	{
		DEBUG_c++;
		if (FD_ISSET(it->socket, &(sp.reading_set)))
		{
			DEBUG_rd++;
			last_read = recv(it->socket, buffer, BUFFER_SIZE, 0);
			if (last_read <= 0 || has_telnet_breaksignal(last_read, buffer))
			{
				shutdown(it->socket, SHUT_RDWR);
				close(it->socket);
				clients.erase(it);
				break;
			}
			it->store_incoming_data(buffer, last_read);
			break;
		}
	}
	// printf("%d clients - %d read\n", DEBUG_c, DEBUG_rd);
}

/*
 * On parcourt les socket client sur le writing set:
 * si une response est prête, on l'envoie
 */
void 	write_to_clients_sockets(SocketPool &sp, std::vector<Client> &clients, std::vector<Server_conf> &server_confs)
{
	int DEBUG_c = 0;
	int DEBUG_wt = 0;
	for (std::vector<Client>::iterator it = clients.begin(); it != clients.end(); it++)
	{
		DEBUG_c++;
		if (FD_ISSET(it->socket, &(sp.writing_set)) && it->requests.front().status == FINISH_PARSING)
		{
			display_request(it->requests.front());
			DEBUG_wt++;
			it->fill_response(server_confs);
			it->send_response();
			it->response.clear();
			if (it->status == 1)
			{
				shutdown(it->socket, SHUT_RDWR);
				close(it->socket);
				clients.erase(it);
			}
			break;
		}
	}
	// printf("%d clients, %d write\n", DEBUG_c, DEBUG_wt);
}

/*
 * args : list de socket listeners initialisé depuis les ports; une liste de clients
 * On ajoute les socket listeners au read set
 * On ajoute les socket clients au read et write set
 *
 * Select sur les write et read set
 * add client si besoin
 * read socket
 * write socket
 *iterator
 */
int 	socket_routine(std::map<int, int> &listen_sockets_pool, std::vector<Client> &clients_pool, std::vector<Server_conf> &server_confs)
{
	SocketPool sp(listen_sockets_pool, clients_pool);
	timeval timeout;
	timeout.tv_sec = 2;
	timeout.tv_usec = 0;

	int waiting_connexions = select(FD_SETSIZE, &sp.reading_set, &sp.writing_set, 0, &timeout);
	// printf("select = %d\n", waiting_connexions);
	if (waiting_connexions == -1)
		return error_and_exit(SOCK_ERR);
	if (!waiting_connexions)
		return (1);

	add_clients(sp, listen_sockets_pool, clients_pool);
	read_from_clients_sockets(sp, clients_pool);
	write_to_clients_sockets(sp, clients_pool, server_confs);
	return 1;
}
