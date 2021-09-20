#include "webserver.hpp"
#include "error.hpp"

using namespace std;

int main(int ac, char **av)
{
	if (ac > 2)
	{
		err_cat error(ARG_ERR);
		return error.act();
	}

	if (conf_parser(av[1]))
		return (1);

	return (0);
}
