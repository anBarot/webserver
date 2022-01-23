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
	memset(&addr, 0,sizeof(addr));
	addr.sin_family= AF_INET;
	for (std::vector<Server_conf>::iterator it = servers_conf.begin(); it != servers_conf.end(); it++)
	{
		fd = socket(AF_INET, SOCK_STREAM, 0);
		if (fd == -1)
		{
			perror(0);
			continue ;
		}
		if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
		{
			perror(0);
			continue ;
		}
		if (!(it->listen_ip.empty()))
		{
			inet_aton(it->listen_ip.c_str(), &ip);
			addr.sin_addr = ip;
		}
		else
			addr.sin_addr.s_addr = htonl(INADDR_ANY);
		addr.sin_port = htons(it->listen_port);
		if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
		{
			perror(0);
			close(fd);
			continue ;
		}
		if (listen(fd, SOMAXCONN) == -1)
		{
			perror(0);
			close(fd);
			continue ;
		}
		FD_SET(fd, &active_rset);
		fd_list.push_back(fd);
		listen_pool[fd].first = it->listen_ip;
		listen_pool[fd].second = it->listen_port;
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
			std::cout << "Connection accepted on fd " << fd << " based on listen fd " << it->first << std::endl;
			FD_SET(fd, &active_rset);
			fd_list.push_back(fd);
			clients.push_back(Client(fd, it->second.second, it->second.first));
		}
	}
	return 0;
}

int Connections::check_clients()
{
	std::vector<Client>::iterator it = clients.begin();

	while (ready_fd)
	{
		std::cout << "Checking clients"<< std::endl;
		if (FD_ISSET(it->socket, &ready_rset))
		{
			std::cout << it->socket << " is ready for reading"<< std::endl;
			--ready_fd;
			if (it->receive_request(servers_conf) == -1)
			{
				std::cout << "Closing connection because of ret 0 on fd " << it->socket << std::endl;
				FD_CLR(it->socket, &active_rset);
				fd_list.remove(it->socket);
				close(it->socket);
				it = clients.erase(it);
			}
			else
			{
				if (it->requests.front().status == FINISH_PARSING)
				{
					FD_SET(it->socket, &active_wset);
					FD_CLR(it->socket, &active_rset);
				}
				++it;
			}
		}
		else if (FD_ISSET(it->socket, &ready_wset) && !it->requests.empty())
		{

			std::cout << it->socket << " is ready for writing"<< std::endl;
			--ready_fd;
			FD_CLR(it->socket, &active_wset);
			if (it->respond(servers_conf) == -1)
			{
				std::cout << "Closing connection because of error on fd " << it->socket << std::endl;
				fd_list.remove(it->socket);
				close(it->socket);
				it = clients.erase(it);
			}
			else
			{
				FD_SET(it->socket, &active_rset);
				++it;
			}
		}
		else
			++it;
	}
	return 0;
}

// void Connections::remove_client(int fd)
// {
// 	close(fd);
// 	fd_list.remove(fd);
// }

void Connections::loop()
{
	struct timeval timeout;
	std::cout << "Waiting for connection." << std::endl;
	
	while (1)
	{
		timeout.tv_sec = 300;
		timeout.tv_usec = 0;
		max_fd = *std::max_element(fd_list.begin(), fd_list.end());
		ready_rset = active_rset;
		ready_wset = active_wset;
		ready_fd = select(max_fd + 1, &ready_rset, &ready_wset, 0, &timeout);
		std::cout << "ready fds " << ready_fd << std::endl;
		if (ready_fd == -1)
			error_and_exit(SOCK_ERR);
		if (ready_fd != 0)
		{
			add_clients();
			check_clients();
		}
		else
		{
			std::vector<Client>::iterator it = clients.begin();
			while (it != clients.end())
			{
				FD_CLR(it->socket, &active_rset);
				FD_CLR(it->socket, &active_wset);
				close(it->socket);
				fd_list.remove(it->socket);
				++it;
			}
			clients.clear();
		}
	}	
}