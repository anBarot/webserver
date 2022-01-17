/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abarot <abarot@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/21 15:20:52 by abarot            #+#    #+#             */
/*   Updated: 2022/01/17 14:21:41 by abarot           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "webserver.hpp"
#include "Response.hpp"

// # include "Response.hpp"

class Client
{
public:
	int socket;
	unsigned short port;
	std::string ip_address;
	int status;
	Response response;
	std::deque<Request> requests;
	std::vector<char> received_data_raw;

	Client(int sock, unsigned short lsock, std::vector<Server_conf> &confs): socket(sock), port(lsock), status(0)
	{
		// client_sv = get_server_conf(confs, port);
		requests.push_back(Request(client_sv.max_body_size));
	}

	~Client(){};

	void store_incoming_data(char buffer[BUFFER_SIZE], int size);
	void extract_request_from_data(std::vector<char> &data);
	void check_payload();
	void check_trailer();
	void check_line();
	void fill_response();
	void fill_error_response();
	void send_response();
};


#endif //CLIENT_HPP
