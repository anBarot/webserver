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
		loc.methods[method_enum] = true;
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
		if (!extract.first.compare("root"))				 
			loc.root = extract.second; 					 
		else if (!extract.first.compare("index"))		 
			loc.index = extract.second;					 
		else if (!extract.first.compare("cgi_path"))	 
			loc.cgi_path = extract.second;				
		else if (!extract.first.compare("cgi_extension"))
			loc.cgi_extension = extract.second;			 
		else if (!extract.first.compare("upload_path"))	 
			loc.upload_path = extract.second;			
		else if (!extract.first.compare("return"))		 
			loc.redirection = extract.second;			 
	}
	else if (!extract.first.compare("method"))					
	{															 
		if (set_and_check_methods(extract.second, loc))
			return (CONFFILE_PARSE_ERR);
	}
	else if (!extract.first.compare("autoindex"))			
	{
		if (!extract.second.compare("on"))					
			loc.auto_index = true;
		else if (!extract.second.compare("off"))			
			loc.auto_index = false;
		else
			return CONFFILE_PARSE_ERR;
	}
	else													
		return CONFFILE_PARSE_ERR;

	return 0;
}

int		extract_server_field(std::string &line, Server_conf &server)
{
	std::pair<std::string, std::string> extract;

	extract = extract_field(line, 1);
	
	if (!extract.first.compare("listen"))
	{
		std::istringstream iss(extract.second);
		std::string num;
		iss >> num;

		server.listen_port = atoi(num.c_str());
	}
	else if (!extract.first.compare("server_name"))
	{
		std::istringstream iss(extract.second);
		std::string word;

		while (iss >> word)
			server.names.push_back(word);
	}
	else if (!extract.first.compare("max_body_size"))
		server.max_body_size = atoi(extract.second.c_str());
	else if (!extract.first.compare("error_page"))
	{
		std::istringstream iss(extract.second);
		std::string num;
		iss >> num;
		std::string file;
		iss >> file;

		server.error_page[atoi(num.c_str())] = file;
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
		if (line.find("\t\t") == 0)			
		{
			if (line.find(" = ") != std::string::npos)	
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
		else if (line.compare(""))							
			return 0;										
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
			if (!line.substr(0, strlen(S_LOCATION)).compare(S_LOCATION))			 
			{
				while (line.substr(0, strlen(S_LOCATION)) == S_LOCATION)
				{
					std::string path;

					if (line.find(":") == std::string::npos) 
						return (CONFFILE_PARSE_ERR);
					path = line.substr(strlen(S_LOCATION), line.length() - strlen(S_LOCATION) - 1);
					server.locations[path] = Location();
					server.locations[path].path = path;

					if (fill_location(conf_file, server.locations[path], line))
						return CONFFILE_PARSE_ERR;
					if (conf_file.eof() == true && !line.size())
						return 0;
				}
			}
			if (line.find(" = ") != std::string::npos)			
			{
				if (extract_server_field(line, server))
					return CONFFILE_PARSE_ERR;	
			}
			else if (line == "server:")						
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
