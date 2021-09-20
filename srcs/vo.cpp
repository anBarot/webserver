#include "../include/webserver.hpp"


int main()
{
	Server server(80);

	listen_from_server(server);

}