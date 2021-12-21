/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_conf.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abarot <abarot@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/21 15:23:34 by abarot            #+#    #+#             */
/*   Updated: 2021/12/21 15:23:44 by abarot           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "libraries.hpp"
#include "Location.hpp"

class Server_conf
{
	public:
		unsigned							listen_port;
		std::list<std::string>				names;
		unsigned							max_body_size;
		std::map<std::string, Location> 	locations;
		std::map<unsigned, std::string>		error_page;
	
		Server_conf() : listen_port(80), max_body_size(1) {}
		Server_conf(int port) : listen_port(port) {}
		~Server_conf() {}
};

#endif //SERVER_HPP
