#include "../include/webserver.hpp"

std::vector<Server> parsing_simulation()
{
	std::vector<Server> servers;
	servers.push_back(Server(8080));
	servers.push_back(Server(8081));
	return servers;
}

void 	set_sockets_in_fd_set(std::vector<int> sockets, fd_set *set)
{
	for(std::vector<int>::iterator it = sockets.begin(); it != sockets.end(); it++)
		FD_SET(*it, set);
}

int 	socket_routine(std::vector<int> listen_sockets_pool)
{
	std::vector<Client> clients_pool;
	fd_set master_set, read_set, write_set;
	int sockets_ready_count;
	int new_fd;
	// prepare the sets
	FD_ZERO(master_set);
	FD_ZERO(write_set);
	set_sockets_in_fd_set(listen_sockets_pool, &master_set);
	memcpy(&read_set, &master_set, sizeof(master_set));

	// select dans les sets
	sockets_ready_count = select(FD_SETSIZE, &working_set, 0, 0, 0);

	// tant qu'il y a des socket à lire on les cherche
	for (int socket_descriptor = 0; socket_descriptor < FD_SETSIZE && sockets_ready_count > 0; socket_descriptor++)
	{
		if (FD_ISSET(socket_descriptor, read_set) && is_value_in_listen_sockets(socket_descriptor, listen_sockets_pool))
		{
			do
			{
				new_fd = accept(socket_descriptor, 0, 0);
				if (new_fd < 0)
				{
					if (errno != EWOULDBLOCK)
						error_and_exit(SOCK_ERR);
					break;
				}
				FD_SET(socket_descriptor, response_set);
				sockets_ready_count--;
			} while (new_fd != -1);
		}
		else if (FD_ISSET(socket_descriptor, read_set))
		{

		}
	}


	// si select > 0
		// iterate parmi les fd voir is is_set
			// traiter les fd is_iset => créer un client
			// ajouter les fd dans le set d'écriture
			// lire le merdier
		// iterate parmi les df d'écriture
			// si c'est pret leur envoyer la data

}

int main()
{
	std::vector<int> listen_sockets_pool = listen_sockets_from_servers(parsing_simulation());


	while(socket_routine(listen_sockets_pool))
	{

	}

}