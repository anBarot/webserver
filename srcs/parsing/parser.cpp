#include <string.h>
#include <stdlib.h>
#include "../../include/webserver.hpp"


using namespace std;

int		fill_location(ifstream &conf_file, Location &loc)
{




	return (0);
}

int fill_server(ifstream &conf_file, Server &server)
{
	string line;

	while (getline(conf_file, line))
	{
		ws_trim(line);
		pair<string, string> extract(line.substr(0, line.find_first_of("=")),
		line.substr(line.find_first_of("=") + 1, line.length()));
		ws_trim(extract.first);
		ws_trim(extract.second);
  		cout << "extract first : " << extract.first << '\n';
  		cout << "extract second : " << extract.second << '\n';
		if (extract.first.compare("listen"))
			server.add_listen_port(atoi(extract.second.c_str()));
		else if (extract.first.compare("server_name"))
			server.add_name(extract.second);
		else if (extract.first.substr(0, strlen("location")).compare("location"))
		{	
			server.add_locations(Location());
			fill_location(conf_file, server.get_location().back());
		}
	}

	return (0);
}

int conf_parser(char *file_name)
{
	ifstream conf_file (file_name);
	string line;
	vector<Server> servers;

  	if (conf_file.is_open()) 
	{
		while (getline(conf_file, line))
		{
			ws_trim(line);
			if (!line.compare("server:"))
			{
				std::cout << "server found\n";
				servers.push_back(Server());
				fill_server(conf_file, servers.back());
			}
		}
		conf_file.close();
  	}
	else
	{
		err_cat error(FILE_ERR);
		return error.act();
	}

	return (0);
}
