//
// Created by lubenica on 21/09/2021.
//

#ifndef WEBSERVER_SOCKETPOOL_HPP
#define WEBSERVER_SOCKETPOOL_HPP

#include "cpp_libraries.hpp"
#include "data_struct.hpp"
#include "Client.hpp"

class SocketPool
{
public:

	SocketPool(std::map<int, int> ls, std::vector<Client> &clients) // ajouter les clients
	{
		FD_ZERO(&reading_set);
		FD_ZERO(&writing_set);

		for (std::map<int, int>::iterator it = ls.begin(); it != ls.end(); it++)
		{
			FD_SET(it->first, &reading_set);
			// printf("added socket listener (%d) to reading set\n", *it);
		}

		for (std::vector<Client>::iterator it = clients.begin(); it != clients.end(); it++)
		{
			// printf("added client (%d) to read and write set\n", it->socket);
			FD_SET(it->socket, &reading_set);
			FD_SET(it->socket, &writing_set);
		}
	};

	fd_set reading_set, writing_set;

	private:
};




#endif //WEBSERVER_SOCKETPOOL_HPP
