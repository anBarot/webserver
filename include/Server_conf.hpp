/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_conf.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abarot <abarot@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/21 15:23:34 by abarot            #+#    #+#             */
/*   Updated: 2022/01/26 16:15:34 by abarot           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP 1

#include "libraries.hpp"
#include "Location.hpp"

class Server_conf
{
	public:
		unsigned short						listen_port;
		std::string							listen_ip;
		std::list<std::string>				names;
		unsigned							max_body_size;
		std::map<std::string, Location> 	locations;
		std::map<unsigned, std::string>		error_page;
	
		Server_conf() : listen_port(80), listen_ip("127.0.0.1"), max_body_size(1) {}
		Server_conf(int port) : listen_port(port) {}
		~Server_conf() {}
};

Server_conf get_server_conf(/*Request &req, */std::vector<Server_conf> &confs, unsigned int lsock);

#endif //SERVER_HPP
