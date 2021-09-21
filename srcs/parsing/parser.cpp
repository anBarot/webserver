#include "../../include/webserver.hpp"

#define S_LOCATION			"\tlocation "
#define FIELD_DELIMITER		" = "
#define DIGITS				"0123456789"

using namespace std;

void	display_server(vector<Server> &servers)
{
	cout << "Check parsing : \n";

	for(vector<Server>::iterator it = servers.begin(); it != servers.end(); ++it)
	{
		for (list<string>::iterator i_name = it->get_name().begin(); i_name != it->get_name().end(); ++i_name)
			cout << "server name : "<< *i_name << "\n";
		
		cout << "listen port : "<< it->get_listen_port() << "\n";
		cout << "max upload size : "<< it->get_max_body_size() << "\n";

		for (list<Location>::iterator loc = it->get_location().begin(); loc != it->get_location().end(); ++loc)
		{
			cout << "\nFor location : "<< (*loc).get_location() << "\n";
			cout << "root: "<< (*loc).get_root() << "\n";
			cout << "auto index : "<< (((*loc).get_auto_index() == true) ? "true" : "false") << "\n";
			cout << "index : "<< (*loc).get_index() << "\n";
			cout << "upload path : "<< (*loc).get_upload_path() << "\n";
			cout << "cgi path : "<< (*loc).get_cgi_path() << "\n";
			cout << "cgi extension : "<< (*loc).get_cgi_extension() << "\n";

			cout << "methods GET : "<< (((*loc).get_methods(GET) == true) ? "true" : "false") << "\n";
			cout << "methods PUT : "<< (((*loc).get_methods(PUT) == true) ? "true" : "false") << "\n";
			cout << "methods DELETE : "<< (((*loc).get_methods(DELETE) == true) ? "true" : "false") << "\n";
			cout << "methods POST : "<< (((*loc).get_methods(POST) == true) ? "true" : "false") << "\n";
		}
	}
}

pair<string, string> extract_field(string &line, unsigned tab_nb)
{
	pair<string, string> extract(line.substr(tab_nb, line.find_first_of(FIELD_DELIMITER) - tab_nb),
	line.substr(line.find_first_of(FIELD_DELIMITER) + 3, line.length()));
  	// cout << "extract first : _" << extract.first << "_ \n";
  	// cout << "extract second : _" << extract.second << "_ \n";
	
	return extract;
}

int		set_and_check_methods(string &extract, Location &loc)
{
	istringstream iss(extract);
	string word;
	list<string> ls; 

	while (iss >> word)
		ls.push_back(word);

	for (list<string>::iterator it = ls.begin(); it != ls.end(); ++it)
	{
		if (!(*it).compare("GET"))
			loc.set_methods(GET, true);
		else if (!(*it).compare("POST"))
			loc.set_methods(POST, true);
		else if (!(*it).compare("PUT"))
			loc.set_methods(PUT, true);
		else if (!(*it).compare("DELETE"))
			loc.set_methods(DELETE, true);
		else
			return (PARSE_ERR);
	}
	return (0);
}

int		fill_location(ifstream &conf_file, Location &loc, string &line)
{
	string location_str;
	pair<string, string> extract;

	if (line.find(":") == string::npos) 
		return (PARSE_ERR);
	
	location_str = line.substr(line.find_first_not_of(S_LOCATION), line.find_first_of(":"));
	loc.set_location(location_str);
		
	while (getline(conf_file, line))
	{
		ws_trim(line);
		if (line.compare("") && line.find("\t\t") == 0)						// not empty line and begins with two tabs
		{
			extract = extract_field(line, 2);
			if (!extract.first.compare("root"))								// extraction field root
				loc.set_root(extract.second); 								// ex : /var/www/
			else if (!extract.first.compare("index"))						// extraction field index
				loc.set_index(extract.second);								// ex : index.php
			else if (!extract.first.compare("method"))						// extraction field methods
			{																// ex : GET PUT DELETE
				if (set_and_check_methods(extract.second, loc))
					return (PARSE_ERR);
			}
			else if (!extract.first.compare("cgi_path"))					// extraction field cgi_path
				loc.set_cgi_path(extract.second);							// ex :	/usr/bin/php-cgi
			else if (!extract.first.compare("cgi_extension"))				// extraction field cgi_extension
				loc.set_cgi_extension(extract.second);						// ex : python
			else if (!extract.first.compare("upload_path"))					// extraction field upload_path
				loc.set_upload_path(extract.second);						// ex : /var/www/put_folder
			else if (!extract.first.compare("autoindex"))					// extraction field autoindex
			{
				if (extract.second.compare("on") != string::npos)			// if on : auto_index = true
					loc.set_auto_index(true);
				else if (extract.second.compare("off") != string::npos)		// if off : auto_index = false
					loc.set_auto_index(false);
				else
					return (PARSE_ERR);
			}
			else															// not an acceptable field
				return (PARSE_ERR);
		}
		else if (line.compare(""))											// line that doesn't begin with two tabs and not an empty line
			return (0);														// return to fill location
	}
	line = "";
	return (0);
}

int fill_server(ifstream &conf_file, Server &server)
{
	string line;
	pair<string, string> extract;

	while (getline(conf_file, line))
	{
		ws_trim(line);
		if (line.compare(""))														
		{
			if (!line.substr(0, strlen(S_LOCATION)).compare(S_LOCATION))			// if location found 
			{
				while (!line.substr(0, strlen(S_LOCATION)).compare(S_LOCATION))		// create new location and fill it
				{
					// cout << "location found\n";
					server.add_locations(Location());
					if (fill_location(conf_file, server.get_location().back(), line))
						return PARSE_ERR;
				}
			}
			if (line.find(" = ") != string::npos)									// if server field found
			{
				extract = extract_field(line, 1);									// extract values and set the according field in Server object
				if (!extract.first.compare("listen"))
				{
					server.add_listen_port(atoi(extract.second.c_str()));
					server.add_name(extract.second.substr(extract.second.find_last_of(DIGITS) + 1,
					extract.second.find_last_not_of("\t\r\n ")));
				}
				else if (!extract.first.compare("server_name"))
					server.add_name(extract.second);
				else if (!extract.first.compare("max_body_size"))
					server.add_max_body(atoi(extract.second.c_str()));
				else
					return (PARSE_ERR);
			}
			else if (!line.compare("server:"))
				return 0;
			else
				return (PARSE_ERR);
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
			while (!line.compare("server:"))
			{
				// cout << "server found\n";
				servers.push_back(Server());
				if (fill_server(conf_file, servers.back()))
					return error_and_exit(PARSE_ERR);
				if (conf_file.eof() == true)
					break;
			}
			if (conf_file.eof() == false && line.compare(""))
				return error_and_exit(PARSE_ERR);
		}
		conf_file.close();
  	}
	else
		return error_and_exit(FILE_ERR);

	display_server(servers);
	return 0;
}
