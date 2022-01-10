#include "../../include/webserver.hpp"

Connections::Connections()
{
}

// Connections::Connections(const Connections &c)
// {
// }

// Connections& Connections::operator=(const Connections &c)
// {
// 	return *this;
// }

Connections::~Connections()
{
}

int Connections::init()
{
	struct sockaddr_in addr;
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

		// verify addresses: should we take them in consideration?
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
		addr.sin_port = htons(it->listen_port);
		bind(fd, (struct sockaddr *)&addr, sizeof(addr));
		//implement error

		listen(fd, SOMAXCONN);
		//implemen error
		FD_SET(fd, &active_rset);
		fd_list.push_back(fd);
		listen_pool[fd] = it->listen_port;
	}
	max_fd = *std::max_element(fd_list.begin(), fd_list.end());
	return 0;
}

int Connections::add_clients()
{
	int fd;

	for (std::map<int, int>::iterator it = listen_pool.begin(); it != listen_pool.end(); it++)
	{
		if (FD_ISSET(it->first, &ready_rset))
		{
			ready_fd--;
			fd = accept(it->first, 0, 0);
			//implement error
			std::cout << "Connection accepted." << std::endl;
			FD_SET(fd, &active_rset);
			fd_list.push_back(fd);
			max_fd = *std::max_element(fd_list.begin(), fd_list.end());
			clients.push_back(Client(fd, it->second, servers_conf));
		}
	}
	return 0;
}

int Connections::check_clients()
{
	char buffer[BUFFER_SIZE];
	ssize_t ret;

	for (std::vector<Client>::iterator it = clients.begin();
		it != clients.end(); it++)
	{
		if (FD_ISSET(it->socket, &ready_rset))
		{
			ready_fd--;
			ret = recv(it->socket, buffer, BUFFER_SIZE, 0);
			if (ret <= 0 || has_telnet_breaksignal(ret, buffer))
			{
				FD_CLR(it->socket, &active_rset);
				fd_list.remove(it->socket);
				max_fd = *std::max_element(fd_list.begin(), fd_list.end());
				close(it->socket);
				it = clients.erase(it);
				it--;
			}
			else
			{
				it->store_incoming_data(buffer, ret);
				if (it->requests.front().status == FINISH_PARSING)
				{
					FD_SET(it->socket, &active_wset);
					FD_CLR(it->socket, &active_rset);
				}
			}
		}
		else if (FD_ISSET(it->socket, &ready_wset) && !it->requests.empty())
		{
			it->fill_response(/*servers_conf*/);
			it->send_response();
			it->response.clear();
			FD_CLR(it->socket, &active_wset);
			if (it->status == 1)
			{
				fd_list.remove(it->socket);
				max_fd = *std::max_element(fd_list.begin(), fd_list.end());
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


void Connections::loop()
{
	std::cout << "Waiting for connection." << std::endl;
	while (1)
	{
		// std::cout << "clients " << clients.size() << std::endl;
		// std::cout << "listen " << listen_pool.size() << std::endl;
		// std::cout << "max_fd " << max_fd << std::endl;
		ready_rset = active_rset;
		ready_wset = active_wset;
		ready_fd = select(max_fd + 1, &ready_rset, &ready_wset, 0, 0);
		if (ready_fd == -1)
			error_and_exit(SOCK_ERR);
		add_clients();
		check_clients();
	}
}