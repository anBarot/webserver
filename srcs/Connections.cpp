#include "Connections.hpp"

// Connections::Connections()
// {
// }

// Connections::Connections(const Connections &c)
// {
// }

// Connections& Connections::operator=(const Connections &c)
// {
// 	return *this;
// }

// Connections::~Connections()
// {
// }


int Connections::init()
{
	struct sockaddr_in addr;
	struct in_addr ip;
	int optval;

	FD_ZERO(&active_rset);
	FD_ZERO(&active_wset);
	optval = 1;
	memset(&addr, 0,sizeof(addr));
	addr.sin_family= AF_INET;
	for (std::vector<Server_conf>::iterator it = servers_conf.begin(); it != servers_conf.end(); it++)
	{
		int fd;

		fd = socket(AF_INET, SOCK_STREAM, 0);
		// implement error
		setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
		// implement error

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
			close(fd);
			continue ;
		}

		if (listen(fd, SOMAXCONN) == -1)
		{
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
			//implement error
			std::cout << "Connection accepted." << std::endl;
			FD_SET(fd, &active_rset);
			fd_list.push_back(fd);
			clients.push_back(Client(fd, it->second.second, it->second.first));
		}
	}
	return 0;
}

int Connections::check_clients()
{
	char buffer[BUFFER_SIZE];
	ssize_t ret;

	for (std::vector<Client>::iterator it = clients.begin();
		it != clients.end(); ++it)
	{
		if (FD_ISSET(it->socket, &ready_rset))
		{
			ready_fd--;
			ret = recv(it->socket, buffer, BUFFER_SIZE - 1, 0);
			if (ret <= 0 || has_telnet_breaksignal(ret, buffer))
			{
				FD_CLR(it->socket, &active_rset);
				fd_list.remove(it->socket);
				close(it->socket);
				it = clients.erase(it);
				it--;
			}
			else
			{
				it->store_incoming_data(buffer, servers_conf);
				if (it->requests.front().status == FINISH_PARSING)
				{
					FD_SET(it->socket, &active_wset);
					FD_CLR(it->socket, &active_rset);
				}
			}
		}
		else if (FD_ISSET(it->socket, &ready_wset) && !it->requests.empty())
		{
			it->respond(servers_conf);
			FD_CLR(it->socket, &active_wset);
			if (it->status == 1)
			{
				fd_list.remove(it->socket);
				close(it->socket);
				it = clients.erase(it);
				it--;
			}
			else
				FD_SET(it->socket, &active_rset);
		}
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
	std::cout << "Waiting for connection." << std::endl;
	struct timeval timeout;

	
	while (1)
	{
		timeout.tv_sec = 30;
		timeout.tv_usec = 0;
		max_fd = *std::max_element(fd_list.begin(), fd_list.end());
		ready_rset = active_rset;
		ready_wset = active_wset;
		ready_fd = select(max_fd + 1, &ready_rset, &ready_wset, 0, &timeout);
		std::cout << ready_fd << std::endl;
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
				fd_list.remove(it->socket);
				close(it->socket);
				it = clients.erase(it);
				if (it != clients.end())
					++it;
			}
		}
	}	
}