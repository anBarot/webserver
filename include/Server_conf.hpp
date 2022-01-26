/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_conf.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abarot <abarot@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/21 15:23:34 by abarot            #+#    #+#             */
/*   Updated: 2022/01/05 16:56:37 by abarot           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

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
		bool								is_virtual;
	
		Server_conf() : listen_port(80), listen_ip("127.0.0.1"), max_body_size(1), is_virtual(false) {}
		Server_conf(int port) : listen_port(port), listen_ip("127.0.0.1"), max_body_size(1), is_virtual(false) {}
		~Server_conf() {}

		struct OpenFile : public std::exception {
			virtual const char* what() const throw() {
				return ("Fail to open file");
			};
		};
		class ConfError : public std::exception {
			public:
				std::string	about;
				std::string	near;
				size_t		line;
				ConfError(std::string a, std::string n = "", size_t l = 0) : about(a), near(n), line(l) {};
				~ConfError() throw() {}
				virtual const char* what() const throw() {
					return (about.c_str());
				};
		};
};

Server_conf get_server_conf(/*Request &req, */std::vector<Server_conf> &confs, unsigned int lsock);

#endif //SERVER_HPP
