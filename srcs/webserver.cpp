/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserver.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abarot <abarot@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/21 15:35:13 by abarot            #+#    #+#             */
/*   Updated: 2022/02/01 19:29:02 by abarot           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserver.hpp"
#include "Connections.hpp"

std::map<int, std::string> reason_phrase;
std::map<std::string, std::string> MIME_types;

void sigint_handler(int signum)
{
	std::cout << "\nQuitting webserver\n";
	exit(signum);
}

int main(int ac, char **av)
{	
	Connections connections;

	if (ac > 2)
		return (error_and_exit(ARG_ERR));
	char *conf_file = (ac == 2 ? (char *)av[1] : (char *)DEFAULT_SERVER_CONF);
	
	try {
		if (conf_parser(conf_file, connections.servers_conf))
			return (error_and_exit(CONFFILE_PARSE_ERR));
	} catch (Server_conf::ConfError &ex) {
		std::cerr << RED << "Error in file "
			<< BOLDCYAN << conf_file
			<< RED << ": "
			<< RESET << ex.what() << std::endl;
		if (!ex.near.empty()) {
			std::cerr << RED << "    ↳ "
				<< YELLOW<< "Near line "
				<< BOLDBLUE << ex.line
				<< YELLOW << ": '"
				<< RESET << ex.near
				<< YELLOW << '\''
				<< RESET << std::endl;
		}
		return (EXIT_FAILURE);
	} catch (Server_conf::OpenFile &ex) {
		std::cerr << RED << ex.what() << RESET << ": " << BOLDCYAN << conf_file << RESET << std::endl;
		return (EXIT_FAILURE);
	} catch (std::exception &ex) {
		std::cerr << "Unknown error " << ex.what() << std::endl;
		return (EXIT_FAILURE);
	}

	maps_init_MIME_types(MIME_types);
	maps_init_reason_phrase(reason_phrase);
	
	signal(SIGINT, sigint_handler);

	try {
		connections.init();
	} catch (std::exception &ex) {
		std::cerr << RED<< "No servers has been launched " << RESET << std::endl;
		return (EXIT_FAILURE);
	}
	__WEBSERV_INTRO;
	connections.loop();

	return (0);
}
