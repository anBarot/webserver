#include "../include/webserver.hpp"


using namespace std;

int main(int ac, char **av)
{
	if (ac > 2)
		return (error_and_exit(ARG_ERR));

	if (conf_parser(av[1]))
		return (1);

	return (0);
}
