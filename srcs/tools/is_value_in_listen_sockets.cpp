#include "../../include/webserver.hpp"

bool	is_value_in_listen_sockets(int fd, std::vector<int> ls)
{
	for (std::vector<int>::iterator it = ls.begin(); it != ls.end(); it++)
	{
		if (*it == fd)
			return true;
	}
	return false;
}