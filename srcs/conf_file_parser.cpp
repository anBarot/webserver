/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   conf_file_parser.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adda-sil <adda-sil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/21 15:27:54 by abarot            #+#    #+#             */
/*   Updated: 2022/01/28 18:23:08 by adda-sil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserver.hpp"

int g_line = 0;

std::ostream &operator << (std::ostream &out, const Server_conf &sc) {
	out << BOLDBLUE << std::endl
		<< "------ Server -----" << RESET << std::endl;

	Server_conf::listenables::const_iterator l_it = sc.listens.begin();
	Server_conf::listenables::const_iterator l_ite = sc.listens.end();
	for (; l_it != l_ite; l_it++) {
		out << " ↳ "
			<< CYAN << l_it->first
			<< RESET << ":"
			<< BOLDCYAN << l_it->second
			<< std::endl << RESET;

	}
	out << BLUE << "------ Names ------" << RESET << std::endl;
	std::list<std::string>::const_iterator n_it = sc.names.begin();
	std::list<std::string>::const_iterator n_ite = sc.names.end();
	for (; n_it != n_ite; n_it++) {
		out << " ↳ " << *n_it << std::endl;
	}
	out << BLUE << "-------------------" << RESET << std::endl;
	out << "isVirtual(" << RED << sc.is_virtual << RESET 
		<< ") " << std::endl
		<< "MaxBodySize(" BLUE << sc.max_body_size << RESET
		<< ")" << std::endl;
	out << BOLDBLUE << "-------------------" << RESET << std::endl;
	return out;
}

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

	if (extract.first == "root" || extract.first == "index" || extract.first == "cgi_path" || extract.first == "upload_path")
	{
		if (extract.second.find_first_of("\t ") != std::string::npos)
				throw(Server_conf::ConfError("location field tab missing", line, g_line));
		if (extract.first == "root")		 
			loc.root = extract.second; 					 
		else if (extract.first == "index")
			loc.index = extract.second;					 
		else if (extract.first == "cgi_path")
			loc.cgi_path = extract.second;				
		 
		else if (extract.first == "upload_path")
			loc.upload_path = extract.second;			
	}
	else if (extract.first == "cgi_extension")
	{
		std::istringstream iss(extract.second);
		std::string str;

		while (iss >> str)
			loc.cgi_extension.push_back(str);
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
		int port = atoi(word.c_str());
		if (port == 80)
			port = 8080;
		if (port < 1024 || port > 49451)
			throw(Server_conf::ConfError("Port is out of range", line, g_line));
		if ((iss >> word))
			server.listen_ip = word;
		else
			server.listen_ip = "127.0.0.1";
		server.listen_port = port;
		try {
			server.addListen(server.listen_ip, server.listen_port);
		} catch (Server_conf::ListenAlreadyExist &ex) {
			throw(Server_conf::ConfError("This listen have already been defined previously", line, g_line));
		}
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
		server.error_page[atoi(num.c_str())] = file;
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

bool conf_exist(std::vector<Server_conf> &confs, Server_conf &c)
{
	for (std::vector<Server_conf>::iterator conf = confs.begin(); conf != confs.end() - 1; ++conf)
		for (Server_conf::listenables::iterator itl = conf->listens.begin(); itl != conf->listens.end(); itl++)
			for (Server_conf::listenables::iterator itc = c.listens.begin(); itc != c.listens.end(); itc++)
				if (itl->first == itc->first && itl->second == itc->second)
					return true;
	return (false);
}

int conf_parser(char *file_name, std::vector<Server_conf> &servers)
{
	std::ifstream conf_file (file_name);
	std::string line;

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
				if (conf_exist(servers, servers.back())) {
					servers.back().is_virtual = true;
				}
				std::cout << "Server registered: " << servers.back() << std::endl;
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
