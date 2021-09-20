#include "../include/webserver.hpp"


int main()
{
	Server server(80);

	int server_socket = socket_from_server(server);

	fd_set current_sockets, ready_sockets;

	FD_ZERO(&current_sockets);
	FD_SET(server_socket, &current_sockets);

	while(1)
	{
		ready_sockets = current_sockets;
		if (select(FD_SETSIZE, &ready_sockets, 0, 0, 0) < 0)
			exit(0);

		for(int i = 0; i < FD_SETSIZE; i++)
		{
			if (FD_ISSET(i, &ready_sockets))
			{
				if (i == server_socket)
				{
					int client_socket = 0;
					FD_SET(client_socket, &current_sockets);
				}
				else
				{
					// serve client
					FD_CLR(i, &current_sockets);
				}
			}
		}
	}

}