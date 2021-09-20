#include <string.h>
#include <stdlib.h>
#include "../../include/webserver.hpp"

using namespace std;

pair<string, string> extract_field(string &line)
{
	pair<string, string> extract(line.substr(1, line.find_first_of(" = ")),
	line.substr(line.find_first_of(" = "), line.length()));
  	cout << "extract first : " << extract.first << '\n';
  	cout << "extract second : " << extract.second << '\n';
	
	return extract;
}

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
		
		if (!line.substr(0, strlen("location")).compare("location"))
		{
			server.add_locations(Location());
			fill_location(conf_file, server.get_location().back());
		}
		else {
			pair<string, string> extract = extract_field(line);
			if (extract.first.compare("listen"))
				server.add_listen_port(atoi(extract.second.c_str()));
			else if (extract.first.compare("server_name"))
				server.add_name(extract.second);
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
			else if (line.length())
				return (error_and_exit(PARSE_ERR));
		}
		conf_file.close();
  	}
	else
		return error_and_exit(FILE_ERR);

	return (0);
}
