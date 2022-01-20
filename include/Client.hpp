/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abarot <abarot@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/21 15:20:52 by abarot            #+#    #+#             */
/*   Updated: 2022/01/20 12:13:43 by abarot           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "webserver.hpp"
# include "Response.hpp"

class Client
{
public:
	int socket;
	unsigned short port;
	std::string ip_address;
	int status;
	std::deque<Request> requests;
	Response response;
	std::vector<char> received_data_raw;

	Client(int sock, unsigned short lsock, std::string n_ip_add): socket(sock), port(lsock), ip_address(n_ip_add), status(0)
	{
		requests.push_back(Request());
	}

	~Client(){};

	void store_incoming_data(char buffer[BUFFER_SIZE], int size, std::vector<Server_conf> confs);
	void extract_request_from_data(std::vector<char> &data, std::vector<Server_conf> confs);
	void check_payload(std::vector<Server_conf> confs);
	void check_trailer();
	void check_line();
	void fill_response(std::vector<Server_conf> confs);
	void fill_error_response();
	void send_response();
};

#endif //CLIENT_HPP
