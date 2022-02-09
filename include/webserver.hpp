/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserver.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abarot <abarot@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/21 15:24:15 by abarot            #+#    #+#             */
/*   Updated: 2022/02/01 19:30:35 by abarot           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERVER_HPP
# define WEBSERVER_HPP

# define MKSTEMP_DEFAULT_TEMPLATE "./tmp/webserv_tmp_XXXXXX"
# define DEFAULT_SERVER_CONF "./conf/webserv_configuration.conf"
# define TIMEOUT 30

# include "libraries.hpp"
# include "data_struct.hpp"

class Request;
class Location;
class Server_conf;

// Tools
int has_telnet_breaksignal(ssize_t last_read, char *buffer);
void maps_init_reason_phrase(std::map<int, std::string> &reason_phrase);
void maps_init_MIME_types(std::map<std::string, std::string> &MIME_types);
void ws_trim(std::string& s);
void strlower(std::string &str);
int error_and_exit(e_error error_nb);
int	is_dir(std::string path);
int is_reg(std::string path);
e_methods get_method_enum(std::string word);
std::string get_method_string(e_methods enm);
int	check_http_version(std::string version);

// Headers
std::string get_date(time_t now);
std::string get_MIME(std::string filename);
std::string get_allow(Location &loc);
int is_header_str(std::string str);

// Parser
int conf_parser(char *file_name, std::vector<Server_conf> &servers);

# include "Request.hpp"
# include "Location.hpp"
# include "Server_conf.hpp"
# include "term.hpp"

#endif
