/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_conf.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adda-sil <adda-sil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/21 15:23:34 by abarot            #+#    #+#             */
/*   Updated: 2022/01/29 16:08:03 by adda-sil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP 1

#include "libraries.hpp"
#include "Location.hpp"
#include "term.hpp"

typedef struct 	s_listener {
	std::string		address;
	unsigned short 	port;
	bool			is_virtual;
}				t_listener;

class Server_conf
{
	public:
		typedef std::list<t_listener>								listenables;
		listenables													listens;
		unsigned short												listen_port;
		std::string													listen_ip;
		std::list<std::string>										names;
		unsigned													max_body_size;
		std::map<std::string, Location> 							locations;
		std::map<unsigned, std::string>								error_page;

		Server_conf() : listen_port(80), listen_ip("127.0.0.1"), max_body_size(1000) {}
		Server_conf(int port) : listen_port(port), listen_ip("127.0.0.1"), max_body_size(1000) {}
		~Server_conf() {}

		struct OpenFile : public std::exception {
			virtual const char* what() const throw() {
				return ("Fail to open file");
			};
		};
		struct ListenAlreadyExist : public std::exception {
			std::string			ip;
			unsigned short		port;
			std::string			name;
			ListenAlreadyExist(std::string i, unsigned short p) :
				ip(i), port(p) {};
			~ListenAlreadyExist() throw() {}
			virtual const char* what() const throw() {
				return ("Listen already exist");
			};
		};
		struct ConfError : public std::exception {
			std::string	about;
			std::string	near;
			size_t		line;
			ConfError(std::string a, std::string n = "", size_t l = 0) : about(a), near(n), line(l) {};
			~ConfError() throw() {}
			virtual const char* what() const throw() {
				return (about.c_str());
			};
		};

		bool isListeningOn(t_listener listen) { return isListeningOn(listen.address, listen.port); }
		bool isListeningOn(std::string address = "127.0.0.1", unsigned short port = 8080) {
			listenables::iterator it 	= this->listens.begin();
			listenables::iterator ite 	= this->listens.end();
			for (; it != ite; it++)
				if (it->address == address && it->port == port)
					return true;
			return false;
		}
		void addListen(t_listener listen) throw(ListenAlreadyExist) { addListen(listen.address, listen.port); }
		void addListen(std::string address = "127.0.0.1", unsigned short port = 8080) throw(ListenAlreadyExist) {
			if (isListeningOn(address, port))
				throw (ListenAlreadyExist(address, port));
			t_listener l = { .address = address, .port = port, .is_virtual = false };
			this->listens.push_back(l);
		}

};

Server_conf get_server_conf(/*Request &req, */std::vector<Server_conf> &confs, unsigned int lsock);
std::ostream &operator << (std::ostream &out, const Server_conf &sc);

#endif //SERVER_HPP
