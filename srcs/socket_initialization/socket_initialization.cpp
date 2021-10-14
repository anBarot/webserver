#include "../../include/webserver.hpp"

int socket_from_server(Server_conf &server_conf)
{
	int server_socket = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in server_address;
	memset(&server_address, 0, sizeof (server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(server_conf.listen_port);
	server_address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

	bind(server_socket, (struct sockaddr *)(&server_address), sizeof(server_address));

	return server_socket;
}

int listen_from_server(Server_conf &server)
{
	int sock = socket_from_server(server);
	int listening = listen(sock, BACKLOG);
	if (listening < 0)
		error_and_exit(SOCK_ERR);
	return  sock;
}

std::vector<int> listen_sockets_from_servers(std::vector<Server_conf> servers_conf)
{
	std::vector<int> listens;

	for (std::vector<Server_conf>::iterator it = servers_conf.begin(); it != servers_conf.end(); it++)
	{
		listens.push_back(listen_from_server(*it));
	}
	return listens;
}