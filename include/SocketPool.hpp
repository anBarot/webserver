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

	SocketPool(std::vector<int> ls, std::vector<Client> clients) // ajouter les clients
	{
		FD_ZERO(&reading_set);
		FD_ZERO(&writing_set);

		for (std::vector<int>::iterator it = ls.begin(); it != ls.end(); it++)
			FD_SET(*it, &reading_set);

		for (std::vector<Client>::iterator it = clients.begin(); it != clients.end(); it++)
		{
			FD_SET(it->socket, &reading_set);
			FD_SET(it->socket, &writing_set);
		}
	};

	fd_set reading_set, writing_set;
private:
};




#endif //WEBSERVER_SOCKETPOOL_HPP
