#ifndef CONNECTIONS_H
# define CONNECTIONS_H

# include "Client.hpp"

class Connections
{
private:
	
	std::vector<Client> clients;
	std::map<int, std::pair<std::string, int> > listen_pool;

	fd_set	active_rset;
	fd_set	active_wset;
	fd_set	ready_rset;
	fd_set	ready_wset;
	int		ready_fd;
	int		max_fd;
	std::list<int> fd_list;

public:
	Connections();
	Connections(const Connections &c);
	Connections& operator=(const Connections &c);
	~Connections();

	int init();
	int add_clients();
	int check_clients();
	void loop();

	std::vector<Server_conf> servers_conf;
};

#endif