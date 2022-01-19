/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abarot <abarot@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/21 15:20:52 by abarot            #+#    #+#             */
/*   Updated: 2022/01/17 19:09:32 by abarot           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "webserver.hpp"
# include "Response.hpp"

class Client
{

private:


public:
	int socket;
	std::string ip_address;
	unsigned short port;
	int status;
	std::deque<Request> requests;
	Response response;
	std::vector<char> received_data_raw;

	Client(int sock, unsigned short lsock, std::string n_ip_add);

	void store_incoming_data(char buffer[BUFFER_SIZE]);
	void extract_request_from_data();
	void check_payload();
	void check_trailer();
	void check_line();
	void fill_response(std::vector<Server_conf> confs);
	void fill_error_response();
	void send_response();

	void respond(std::vector<Server_conf> &confs);

};

#endif //CLIENT_HPP
