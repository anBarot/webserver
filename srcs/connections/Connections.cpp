#include "../../include/webserver.hpp"

Connections::Connections()
{
}

Connections::Connections(const Connections &c)
{
}

Connections& Connections::operator=(const Connections &c)
{
	return *this;
}

Connections::~Connections()
{
}

int Connections::init(std::vector<Server_conf> servers_conf)
{
	struct sockaddr_in addr;
	int optval;

	FD_ZERO(&active_set);
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

		listen(fd,SOMAXCONN);
		//implemen error

		FD_SET(fd, &active_set);
		max_fd = fd;

		socket_pool[fd] = it->listen_port;
	}
	return 0;
}

void Connections::loop(std::vector<Server_conf> servers_conf)
{
	int status;

	status = 1;
	while (status)
	{
		ready_rset = active_set;
		ready_fd = select(max_fd, &ready_rset, 0, 0, 0);
			
	}
}