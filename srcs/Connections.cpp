/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connections.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abarot <abarot@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/28 05:10:51 by adda-sil          #+#    #+#             */
/*   Updated: 2022/02/08 18:58:01 by abarot           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Connections.hpp"

void Connections::get_addresses()
 {
	for (std::vector<Server_conf>::iterator it = virtual_hosts.begin(); it != virtual_hosts.end(); ++it)
	{
		for (Server_conf::listenables::iterator itl = it->listens.begin(); itl != it->listens.end(); ++itl)
			addresses.insert(std::make_pair(itl->address, itl->port));
	}
 }

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
	
	get_addresses();
	for (std::set<std::pair<std::string, unsigned short> >::iterator it = addresses.begin(); it != addresses.end(); ++it)
	{
		std::string address = it->first;
		unsigned short port = it->second;

		#ifdef LOGGER
			std::cout << CYAN << "Trying to bind " << address << ":" << port << " ... " << std::endl;
		#endif
	
		fd = socket(AF_INET, SOCK_STREAM, 0);
		if (fd == -1)
		{
			std::cerr << "AF_INET: " << errno << "\n" << RED;
			perror(0);
			std::cerr << RESET;
			continue ;
		}
		if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
		{
			std::cerr << "SOL_SOCKET: " << RED;
			perror(0);
			std::cerr << RESET;
			continue ;
		}
		inet_aton(address.c_str(), &ip);
		addr.sin_addr = ip;
		addr.sin_port = htons(port);
		if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
		{
			std::cerr << "HTONS: " << RED;
			perror(0);
			std::cerr << RESET;
			close(fd);
			continue ;
		}
		if (listen(fd, SOMAXCONN) == -1)
		{
			std::cerr << "SOMAXCONN: " << RED;
			perror(0);
			std::cerr << RESET;
			close(fd);
			continue ;
		}
		std::cerr << RESET << std::endl;
		FD_SET(fd, &active_rset);
		fd_list.push_back(fd);
		listen_pool[fd].first = address;
		listen_pool[fd].second = port;
	}
	if (listen_pool.size() == 0 || listen_pool.size() > 64)
		throw std::exception();
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
				#ifdef LOGGER
					std::cerr << RED;
					perror(0);
					std::cerr << RESET;
				#endif // DEBUG
				continue ;
			}
			
			#ifdef LOGGER
				std::cout << MAGENTA << "Connection opened on socket descriptor " << fd << RESET << std::endl;
			#endif // DEBUG

			FD_SET(fd, &active_rset);
			fd_list.push_back(fd);
			clients.push_back(Client(fd, it->second.second, it->second.first, time(NULL)));
		}
	}
	return 0;
}

int Connections::check_clients()
{
	unsigned long	i;
	int				ret;
	
	i = 0;
	while (i < clients.size())
	{
		if (FD_ISSET(clients[i].socket, &ready_rset))
		{
			--ready_fd;
			if (clients[i].receive_request(virtual_hosts) == -1)
				remove_client(i);
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
			ret = clients[i].respond();
			if (ret == -1)
				remove_client(i);
			else
			{
				if (ret == 0)
				{
					FD_CLR(clients[i].socket, &active_wset);
					FD_SET(clients[i].socket, &active_rset);
				}
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
	#ifdef LOGGER
		std::cout << MAGENTA << "Connection closed on socket descriptor " << clients[i].socket << std::endl;
	#endif // DEBUG

	FD_CLR(clients[i].socket, &active_rset);
	FD_CLR(clients[i].socket, &active_wset);
	fd_list.remove(clients[i].socket);
	close(clients[i].socket);
	clients.erase(clients.begin() + i);
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
		{
			#ifdef LOGGER
				std::cerr << RED;
				perror(0);
				std::cerr << RESET;
			#endif // DEBUG
		}
		if (ready_fd != 0)
			add_clients();
		check_clients();
	}
}