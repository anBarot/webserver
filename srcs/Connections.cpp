/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connections.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adda-sil <adda-sil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/28 05:10:51 by adda-sil          #+#    #+#             */
/*   Updated: 2022/01/29 18:37:05 by adda-sil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Connections.hpp"

int Connections::init()
{
	struct sockaddr_in addr;
	struct in_addr ip;
	int optval;
	int fd;

	FD_ZERO(&active_rset);
	FD_ZERO(&active_wset);
	optval = 1;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	for (std::vector<Server_conf>::iterator it = servers_conf.begin(); it != servers_conf.end(); it++)
	{
		for (Server_conf::listenables::iterator itl = it->listens.begin(); itl != it->listens.end(); itl++) {
			std::string address = itl->address;
			unsigned short port = itl->port;
			#ifdef LOGGER
				std::cout << CYAN << "Trying to bind " << address << ":" << port << " ... ";
			#endif
			if (itl->is_virtual) {
				#ifdef LOGGER
					std::cout << MAGENTA <<  "(Virtual skipped)" << std::endl << RESET;
				#endif
				continue ;
			}
			#ifdef LOGGER
				std::cout << std::endl;
			#endif

			fd = socket(AF_INET, SOCK_STREAM, 0);
			if (fd == -1)
			{
				perror(0); // What doe that mean
				continue ;
			}
			if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
			{
				perror(0); // What doe that mean
				continue ;
			}
			if (!(address.empty()))
			{
				inet_aton(address.c_str(), &ip);
				addr.sin_addr = ip;
			}
			else
				addr.sin_addr.s_addr = htonl(INADDR_ANY);
			addr.sin_port = htons(port);
			if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
			{
				#ifdef LOGGER
					std::cerr << YELLOW << "Cannot bind " << BLUE << address << ":" << port << YELLOW << " already used" << RESET << std::endl;
				#endif
				close(fd);
				continue ;
			}
			if (listen(fd, SOMAXCONN) == -1)
			{
				perror(0); // What doe that mean
				close(fd);
				continue ;
			}
			FD_SET(fd, &active_rset);
			fd_list.push_back(fd);
			listen_pool[fd].first = address;
			listen_pool[fd].second = port;
		}
	}
	return 0;
}

int Connections::add_clients()
{
	int fd;

	for (std::map<int, std::pair<std::string, int> >::iterator it = listen_pool.begin(); it != listen_pool.end(); it++)
	{
		if (FD_ISSET(it->first, &ready_rset))
		{
			ready_fd--;
			fd = accept(it->first, 0, 0);
			if (fd == -1)
			{
				perror(0);
				continue ;
			}
			// std::cout << "Connection accepted on fd " << fd << " based on listen fd " << it->first << std::endl;
			FD_SET(fd, &active_rset);
			fd_list.push_back(fd);
			clients.push_back(Client(fd, it->second.second, it->second.first, time(NULL)));
		}
	}
	return 0;
}

int Connections::check_clients()
{
	unsigned long i;

	i = 0;
	while (i < clients.size())
	{
		if (FD_ISSET(clients[i].socket, &ready_rset))
		{
			--ready_fd;
			if (clients[i].receive_request(servers_conf) == -1)
			{
				FD_CLR(clients[i].socket, &active_rset);
				remove_client(i);
			}
			else
			{
				if (clients[i].requests.front().status == FINISH_PARSING)
				{
					FD_SET(clients[i].socket, &active_wset);
					FD_CLR(clients[i].socket, &active_rset);
				}
				clients[i].last_activity = time(NULL);
				++i;
			}
		}
		else if (FD_ISSET(clients[i].socket, &ready_wset))
		{
			--ready_fd;
			FD_CLR(clients[i].socket, &active_wset);
			if (clients[i].respond(servers_conf) == -1)
				remove_client(i);
			else
			{
				FD_SET(clients[i].socket, &active_rset);
				clients[i].last_activity = time(NULL);
				++i;
			}
		}
		else
		{
			if (difftime(time(NULL), clients[i].last_activity) >= TIMEOUT)
				remove_client(i);
			else
				++i;
		}
	}
	return 0;
}

void Connections::remove_client(int i)
{
	fd_list.remove(clients[i].socket);
	close(clients[i].socket);
	clients.erase(clients.begin() + i);
	std::cout << MAGENTA << "Connection closed" << std::endl;
}

void Connections::loop()
{
	struct timeval timeout;
	
	while (1)
	{
		timeout.tv_sec = 30;
		timeout.tv_usec = 0;
		max_fd = *std::max_element(fd_list.begin(), fd_list.end());
		ready_rset = active_rset;
		ready_wset = active_wset;
		__AWAIT_REQ;
		ready_fd = select(max_fd + 1, &ready_rset, &ready_wset, 0, &timeout);
		if (ready_fd == -1)
			error_and_exit(SOCK_ERR);
		if (ready_fd != 0)
			add_clients();
		check_clients();
	}
}