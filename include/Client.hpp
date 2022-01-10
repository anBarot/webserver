/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abarot <abarot@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/21 15:20:52 by abarot            #+#    #+#             */
/*   Updated: 2022/01/10 14:47:01 by abarot           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Request.hpp"
#include "Response.hpp"
#include "Location.hpp"
#include "Server_conf.hpp"
#include "libraries.hpp"

class Client
{
public:
	int socket;
	int lsocket;
	int status;
	Server_conf client_sv;
	Response response;
	std::deque<Request> requests;
	std::vector<char> received_data_raw;

	Client(int sock, int lsock, std::vector<Server_conf> &confs): socket(sock), lsocket(lsock), status(0)
	{
		client_sv = get_server_conf(confs, lsocket);
		requests.push_back(Request(client_sv.max_body_size));
	}

	~Client(){};

	void store_incoming_data(char buffer[BUFFER_SIZE], int size);
	void extract_request_from_data(std::vector<char> &data);
	void check_payload();
	void check_trailer();
	void check_line();
	void fill_response(/*std::vector<Server_conf> &confs*/);
	void fill_error_response();
	void send_response();
};

#endif //CLIENT_HPP
