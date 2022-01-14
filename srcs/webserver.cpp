/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserver.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abarot <abarot@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/21 15:35:13 by abarot            #+#    #+#             */
/*   Updated: 2022/01/05 17:17:26 by abarot           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserver.hpp"

std::map<int, std::string> reason_phrase;
std::map<std::string, std::string> MIME_types;

void sigint_handler(int signum)
{
	std::cout << "\nQuitting webserver\n";
	
	//add close sockets

	exit(signum);
}

int main(int ac, char **av)
{	
	Connections connections;

	if (ac > 2)
		return (error_and_exit(ARG_ERR));

	if (conf_parser(av[1], connections.servers_conf))
		return (error_and_exit(CONFFILE_PARSE_ERR));

	maps_init_MIME_types(MIME_types);
	maps_init_reason_phrase(reason_phrase);
	
	signal(SIGINT, sigint_handler);

	connections.init();
	connections.loop();

	return (0);
}
