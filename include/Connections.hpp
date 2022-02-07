#ifndef CONNECTIONS_H
# define CONNECTIONS_H

# include "Client.hpp"

class Connections
{
	public:
		typedef std::map<int, std::pair<std::string, int> > pool;
	private:
		std::vector<Client> clients;
		std::set<std::pair<std::string, unsigned short> > addresses;
		pool	listen_pool;
		fd_set	active_rset;
		fd_set	active_wset;
		fd_set	ready_rset;
		fd_set	ready_wset;
		int		ready_fd;
		int		max_fd;
		std::list<int> fd_list;

	public:
		int 	init();
		void	get_addresses();
		int 	add_clients();
		int 	check_clients();
		void 	remove_client(int i);
		void 	loop();

		std::vector<Server_conf> virtual_hosts;
};

#endif