#ifndef CONNECTIONS_H
# define CONNECTIONS_H

# include "../include/webserver.hpp"

class Connections
{
private:
	
	std::vector<Client> clients;
	std::map<int, int> socket_pool;

	fd_set	active_set;
	fd_set	ready_rset;
	fd_set	ready_wset;
	int		ready_fd;
	int		max_fd;

public:
	Connections();
	Connections(const Connections &c);
	Connections& operator=(const Connections &c);
	~Connections();

	int init(std::vector<Server_conf> servers_conf);
	// int add_client(Server &server);
	// int check_clients();
	void loop(std::vector<Server_conf> servers_conf);

	std::list<int> fd_list;
};

#endif