#include "../../include/webserver.hpp"


int socket_from_server(Server &server)
{
	int server_socket = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(server.get_listen_port());
	server_address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

	bind(server_socket, (struct sockaddr *)(&server_address), sizeof (server_address));

	return server_socket;
}

int listen_from_server(Server &server)
{
	int sock = socket_from_server(server);
	int listening = listen(sock, BACKLOG);
	if (listening < 0)
		exit(0);
	return  listening;
}