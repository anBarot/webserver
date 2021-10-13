#include "../../include/webserver.hpp"

# define S_LOCATION			"\tlocation "
# define FIELD_DELIMITER	" = "
# define DIGITS				"0123456789"

std::pair<std::string, std::string> extract_field(std::string &line, unsigned tab_nb)
{
	std::pair<std::string, std::string> extract(line.substr(tab_nb, line.find_first_of(FIELD_DELIMITER) - tab_nb),
	line.substr(line.find_first_of(FIELD_DELIMITER) + 3, line.length()));
	return extract;
}

int		set_and_check_methods(std::string &extract, Location &loc)
{
	std::istringstream iss(extract);
	std::string word;
	e_methods method_enum;

	while (iss >> word)
	{
		method_enum = get_method_enum(word);

		if (method_enum == NOT_A_METHOD)
			return (CONFFILE_PARSE_ERR);
		loc.set_methods(method_enum, true);
	}
	return (0);
}

int		extract_location_field(std::string &line, Location &loc)
{
	std::pair<std::string, std::string> extract;

	extract = extract_field(line, 2);

	if (!extract.first.compare("root") || !extract.first.compare("index") || !extract.first.compare("cgi_path")
	|| !extract.first.compare("cgi_extension") || !extract.first.compare("upload_path") || !extract.first.compare("return"))
	{
		if (extract.second.find_first_of("\t ") != std::string::npos)
				return CONFFILE_PARSE_ERR;
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
			return (CONFFILE_PARSE_ERR);
	}
	else if (!extract.first.compare("autoindex"))			// extraction field autoindex
	{
		if (!extract.second.compare("on"))					// if on : auto_index = true
			loc.set_auto_index(true);
		else if (!extract.second.compare("off"))			// if off : auto_index = false
			loc.set_auto_index(false);
		else
			return CONFFILE_PARSE_ERR;
	}
	else													// else : not an acceptable field
		return CONFFILE_PARSE_ERR;

	return 0;
}

int		extract_server_field(std::string &line, Server_conf &server)
{
	std::pair<std::string, std::string> extract;

	extract = extract_field(line, 1);								// extract values and set the according field in Server object
	
	if (!extract.first.compare("listen"))
	{
		std::istringstream iss(extract.second);
		std::string num;
		iss >> num;

		server.add_listen_port(atoi(num.c_str()));
	}
	else if (!extract.first.compare("server_name"))
	{
		std::istringstream iss(extract.second);
		std::string word;

		while (iss >> word)
			server.add_name(word);
	}
	else if (!extract.first.compare("max_body_size"))
		server.add_max_body(atoi(extract.second.c_str()));
	else if (!extract.first.compare("error_page"))
	{
		std::istringstream iss(extract.second);
		std::string num;
		iss >> num;
		std::string file;
		iss >> file;

		server.add_error_page(atoi(num.c_str()), file);
	}
	else
		return CONFFILE_PARSE_ERR;

	return 0;
}

int		fill_location(std::ifstream &conf_file, Location &loc, std::string &line)
{
	while (getline(conf_file, line))
	{
		ws_trim(line);
		if (line.find("\t\t") == 0)			// if line begins with two tabs
		{
			if (line.find(" = ") != std::string::npos)	// if field found : extract field
			{
				if (extract_location_field(line, loc))
					return CONFFILE_PARSE_ERR;
				line.clear();
				if (conf_file.eof() == true)
					return 0;
			}
			else
				return CONFFILE_PARSE_ERR;
		}
		else if (line.compare(""))							// line that doesn't begin with two tabs and not an empty line
			return 0;										// return to fill server
	}
	return 0;
}

int fill_server(std::ifstream &conf_file, Server_conf &server)
{
	std::string line;

	while (getline(conf_file, line) && conf_file.eof() != true)
	{
		ws_trim(line);
		if (line != "")
		{
			if (!line.substr(0, strlen(S_LOCATION)).compare(S_LOCATION))			// if location found 
			{
				while (line.substr(0, strlen(S_LOCATION)) == S_LOCATION)		// create new location and fill it
				{
					std::string location_str;

					if (line.find(":") == std::string::npos) 
						return (CONFFILE_PARSE_ERR);
					location_str = line.substr(strlen(S_LOCATION), line.length() - strlen(S_LOCATION) - 1);
					server.add_locations(location_str, Location());

					if (fill_location(conf_file, server.get_location(location_str), line))
						return CONFFILE_PARSE_ERR;
					if (conf_file.eof() == true && !line.size())
						return 0;
				}
			}
			if (line.find(" = ") != std::string::npos)			// if server field found, extract server field
			{
				if (extract_server_field(line, server))
					return CONFFILE_PARSE_ERR;	
			}
			else if (line == "server:")						// if new server found, go back to creating server loop
				return 0;
			else
				return CONFFILE_PARSE_ERR;
		}
	}
	return 0;
}

int conf_parser(char *file_name, std::vector<Server_conf> &servers)
{
	std::ifstream conf_file (file_name);
	std::string line;

  	if (conf_file.is_open())
	{
		while (getline(conf_file, line))
		{
			ws_trim(line);
			while (!line.compare("server:"))
			{
				servers.push_back(Server_conf());
				if (fill_server(conf_file, servers.back()))
					return error_and_exit(CONFFILE_PARSE_ERR);
				if (conf_file.eof() == true)
				{
					line.clear();
					break;
				}
			}
			if (conf_file.eof() == false && line.compare(""))
				return error_and_exit(CONFFILE_PARSE_ERR);
		}
		conf_file.close();
  	}
	else
		return error_and_exit(FILE_ERR);

	return 0;
}
