#include "../../include/webserver.hpp"

#define S_LOCATION			"\tlocation "
#define FIELD_DELIMITER		" = "
#define DIGITS				"0123456789"

using namespace std;

void	display_server(vector<Server_conf> &servers)
{
	cout << "Check parsing : \n";

	for(vector<Server_conf>::iterator it = servers.begin(); it != servers.end(); ++it)
	{
		cout << "\n\nNew server conf\n";

		for (list<string>::iterator i_name = it->get_name().begin(); i_name != it->get_name().end(); ++i_name)
			cout << "server name : "<< *i_name << "\n";
		
		cout << "listen port : "<< it->get_listen_port() << "\n";
		cout << "max upload size : "<< it->get_max_body_size() << "\n";
		try {
			cout << "error page 404 : "<< it->get_error_page(404) << "\n";
		}
		catch(exception e) { cout << "no error page 404\n"; }

		for (list<Location>::iterator loc = it->get_location().begin(); loc != it->get_location().end(); ++loc)
		{
			cout << "\nFor location : "<< (*loc).get_path() << "\n";
			cout << "root: "<< (*loc).get_root() << "\n";
			cout << "auto index : "<< (((*loc).get_auto_index() == true) ? "true" : "false") << "\n";
			cout << "index : "<< (*loc).get_index() << "\n";
			cout << "upload path : "<< (*loc).get_upload_path() << "\n";
			cout << "cgi path : "<< (*loc).get_cgi_path() << "\n";
			cout << "cgi extension : "<< (*loc).get_cgi_extension() << "\n";
			cout << "redirection : "<< (*loc).get_redirection() << "\n";

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

	while (iss >> word)
	{
		if (!word.compare("GET"))
			loc.set_methods(GET, true);
		else if (!word.compare("POST"))
			loc.set_methods(POST, true);
		else if (!word.compare("PUT"))
			loc.set_methods(PUT, true);
		else if (!word.compare("DELETE"))
			loc.set_methods(DELETE, true);
		else
			return (PARSE_ERR);
	}
	return (0);
}

int		extract_location_field(string &line, Location &loc)
{
	pair<string, string> extract;

	extract = extract_field(line, 2);

	if (!extract.first.compare("root") || !extract.first.compare("index") || !extract.first.compare("cgi_path")
	|| !extract.first.compare("cgi_extension") || !extract.first.compare("upload_path") || !extract.first.compare("return"))
	{
		if (extract.second.find_first_of("\t ") != string::npos)
				return PARSE_ERR;
		if (!extract.first.compare("root"))						// extraction field root
			loc.set_root(extract.second); 						// ex : /var/www/	
		else if (!extract.first.compare("index"))				// extraction field index
			loc.set_index(extract.second);						// ex : index.php
		else if (!extract.first.compare("cgi_path"))			// extraction field cgi_path
			loc.set_cgi_path(extract.second);					// ex :	/usr/bin/php-cgi
		else if (!extract.first.compare("cgi_extension"))		// extraction field cgi_extension
			loc.set_cgi_extension(extract.second);				// ex : python
		else if (!extract.first.compare("upload_path"))			// extraction field upload_path
			loc.set_upload_path(extract.second);				// ex : /var/www/put_folder
		else if (!extract.first.compare("return"))				// extraction field redirection
			loc.set_redirection(extract.second);				// ex : 404
	}
	else if (!extract.first.compare("method"))					// extraction field methods
	{															// ex : GET PUT DELETE
		if (set_and_check_methods(extract.second, loc))
			return (PARSE_ERR);
	}
	else if (!extract.first.compare("autoindex"))			// extraction field autoindex
	{
		if (!extract.second.compare("on"))					// if on : auto_index = true
			loc.set_auto_index(true);
		else if (!extract.second.compare("off"))			// if off : auto_index = false
			loc.set_auto_index(false);
		else
			return PARSE_ERR;
	}
	else													// else : not an acceptable field
		return PARSE_ERR;

	return 0;
}

int		extract_server_field(string &line, Server_conf &server)
{
	pair<string, string> extract;

	extract = extract_field(line, 1);								// extract values and set the according field in Server object
	
	if (!extract.first.compare("listen"))
	{
		istringstream iss(extract.second);
		string num;
		iss >> num;
		string host;
		iss >> host;

		server.add_listen_port(atoi(num.c_str()));
		server.add_name_front(host);
	}
	else if (!extract.first.compare("server_name"))
	{
		istringstream iss(extract.second);
		string word;

		while (iss >> word)
			server.add_name(word);
	}
	else if (!extract.first.compare("max_body_size"))
		server.add_max_body(atoi(extract.second.c_str()));
	else if (!extract.first.compare("error_page"))
	{
		istringstream iss(extract.second);
		string num;
		iss >> num;
		string file;
		iss >> file;

		server.add_error_page(atoi(num.c_str()), file);
	}
	else
		return PARSE_ERR;

	return 0;
}

int		fill_location(ifstream &conf_file, Location &loc, string &line)
{
	string location_str;

	if (line.find(":") == string::npos) 
		return (PARSE_ERR);
	
	location_str = line.substr(line.find_first_not_of(S_LOCATION), line.length() - line.find_first_not_of(S_LOCATION) - 1);
	loc.set_path(location_str);
		
	while (getline(conf_file, line))
	{
		ws_trim(line);
		if (line.find("\t\t") == 0)			// if line begins with two tabs
		{
			if (line.find(" = ") != string::npos)	// if field found : extract field
			{
				if (extract_location_field(line, loc))
					return PARSE_ERR;
				line.clear();
				if (conf_file.eof() == true)
					return 0;
			}
			else
				return PARSE_ERR;
		}
		else if (line.compare(""))							// line that doesn't begin with two tabs and not an empty line
			return 0;										// return to fill server
	}
	return 0;
}

int fill_server(ifstream &conf_file, Server_conf &server)
{
	string line;

	while (getline(conf_file, line) && conf_file.eof() != true)
	{
		ws_trim(line);
		if (line.compare(""))	
		{
			if (!line.substr(0, strlen(S_LOCATION)).compare(S_LOCATION))			// if location found 
			{
				while (!line.substr(0, strlen(S_LOCATION)).compare(S_LOCATION))		// create new location and fill it
				{
					server.add_locations(Location());
					if (fill_location(conf_file, server.get_location().back(), line))
						return PARSE_ERR;
					if (conf_file.eof() == true && !line.size())
						return 0;
				}
			}
			if (line.find(" = ") != string::npos)			// if server field found, extract server field
			{
				if (extract_server_field(line, server))
					return PARSE_ERR;	
			}
			else if (!line.compare("server:"))				// if new server found, go back to creating server loop
				return 0;
			else
				return PARSE_ERR;
		}
	}
	return 0;
}

int conf_parser(char *file_name)
{
	ifstream conf_file (file_name);
	string line;
	vector<Server_conf> servers;

  	if (conf_file.is_open())
	{
		while (getline(conf_file, line))
		{
			ws_trim(line);
			while (!line.compare("server:"))
			{
				servers.push_back(Server_conf());
				if (fill_server(conf_file, servers.back()))
					return error_and_exit(PARSE_ERR);
				if (conf_file.eof() == true)
				{
					line.clear();
					break;
				}
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
