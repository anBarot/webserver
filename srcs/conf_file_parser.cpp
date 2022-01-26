/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   conf_file_parser.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adda-sil <adda-sil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/21 15:27:54 by abarot            #+#    #+#             */
/*   Updated: 2022/01/26 16:54:59 by adda-sil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserver.hpp"

int g_line = 0;

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
			throw(Server_conf::ConfError("method type"));
		loc.methods[method_enum] = true;
	}
	return (0);
}

int		extract_location_field(std::string &line, Location &loc)
{
	std::pair<std::string, std::string> extract;

	extract = extract_field(line, 2);

	if (extract.first == "root" || extract.first == "index" || extract.first == "cgi_path"
	|| extract.first == "cgi_extension" || extract.first == "upload_path")
	{
		if (extract.second.find_first_of("\t ") != std::string::npos)
				throw(Server_conf::ConfError("location field tab missing", line, g_line));
		if (extract.first == "root")		 
			loc.root = extract.second; 					 
		else if (extract.first == "index")
			loc.index = extract.second;					 
		else if (extract.first == "cgi_path")
			loc.cgi_path = extract.second;				
		else if (extract.first == "cgi_extension")
			loc.cgi_extension = extract.second;			 
		else if (extract.first == "upload_path")
			loc.upload_path = extract.second;			
	}
	else if (extract.first == "return")
	{
		std::istringstream iss(extract.second);
		std::string str;

		iss >> str;
		loc.redirection.first = (e_response_code)atoi(str.c_str());
		iss >> str;
		loc.redirection.second = str;
	}
	else if (extract.first == "method")					
	{															 
		if (set_and_check_methods(extract.second, loc))
			throw(Server_conf::ConfError("unknown method", line, g_line));
	}
	else if (extract.first == "autoindex")
	{
		if (extract.second == "on")		
			loc.auto_index = true;
		else if (extract.second == "off")
			loc.auto_index = false;
		else
			throw(Server_conf::ConfError("wrong autoindex value (on|off)", line, g_line));
	}
	else													
		return CONFFILE_PARSE_ERR;

	return 0;
}

int		extract_server_field(std::string &line, Server_conf &server)
{
	std::pair<std::string, std::string> extract;

	extract = extract_field(line, 1);
	
	if (extract.first == "listen")
	{
		std::istringstream iss(extract.second);
		std::string word;
		
		iss >> word;
		server.listen_port = atoi(word.c_str());
		iss >> word;
		server.listen_ip = word;
	}
	else if (extract.first == "server_name")
	{
		std::istringstream iss(extract.second);
		std::string word;

		while (iss >> word)
			server.names.push_back(word);
	}
	else if (extract.first == "max_body_size")
		server.max_body_size = (atoi(extract.second.c_str()) * 1000);
	else if (extract.first == "error_page")
	{
		std::istringstream iss(extract.second);
		std::string num;
		iss >> num;
		std::string file;
		iss >> file;
		std::cerr << "Before err" << std::endl;
		server.error_page[atoi(num.c_str())] = file;
		std::cerr << "After err" << std::endl;
	}
	else {
		throw(Server_conf::ConfError("server format error", line, g_line));
	}
	return 0;
}

int		fill_location(std::ifstream &conf_file, Location &loc, std::string &line)
{
	while (++g_line && getline(conf_file, line))
	{
		ws_trim(line);
		if (line.find("\t\t") == 0)			
		{
			if (line.find(" = ") != std::string::npos)	
			{
				if (extract_location_field(line, loc))
					throw(Server_conf::ConfError("wrong location formating", line, g_line));
				line.clear();
				if (conf_file.eof() == true)
					return 0;
			}
			else
				throw(Server_conf::ConfError("wrong autoindex formating", line, g_line));
		}
		else if (line != "") {
			return (0);
			// throw(Server_conf::ConfError("missing tab for location", line, g_line));
		}
	}
	return 0;
}

int fill_server(std::ifstream &conf_file, Server_conf &server)
{
	std::string line;

	while (++g_line && getline(conf_file, line) && conf_file.eof() != true)
	{
		ws_trim(line);
		if (line != "")
		{
			if (line.substr(0, strlen(S_LOCATION)) == S_LOCATION)		 
			{
				while (line.substr(0, strlen(S_LOCATION)) == S_LOCATION)
				{
					std::string path;

					if (line.find(":") == std::string::npos) 
						throw(Server_conf::ConfError("missing token `:`", line, g_line));
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
					throw Server_conf::ConfError("missing token `=`", line, g_line);	
			}
			else if (line == "server:")			
				return 0;
			else
				throw Server_conf::ConfError("filling server error", line, g_line);
		}
	}
	return 0;
}

int conf_parser(char *file_name, std::vector<Server_conf> &servers)
{
	std::ifstream conf_file (file_name);
	std::string line;

	std::cout << GREEN << "Opening " << file_name << RESET << std::endl;
  	if (conf_file.is_open())
	{
		while (++g_line && getline(conf_file, line))
		{
			ws_trim(line);
			while (line == "server:")
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
			if (conf_file.eof() == false && line != "")
				return error_and_exit(CONFFILE_PARSE_ERR);
		}
		conf_file.close();
  	}
	else {
		throw (Server_conf::OpenFile());
	}

	return 0;
}
