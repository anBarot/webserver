/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abarot <abarot@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/21 15:20:52 by abarot            #+#    #+#             */
/*   Updated: 2022/01/20 15:27:40 by abarot           ###   ########.fr       */
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
	std::string ip_address;
	unsigned short port;
	int status;
	std::deque<Request> requests;
	Response response;
	std::vector<char> received_data_raw;

	Client(int sock, unsigned short lsock, std::string n_ip_add);

	int		receive_request(std::vector<Server_conf> &confs);
	void	respond(std::vector<Server_conf> &confs);

private:
	void extract_request_from_data(std::vector<Server_conf> confs);
	void check_payload(std::vector<Server_conf> confs);
	void check_trailer();
	void check_line();
	void fill_response(std::vector<Server_conf> confs);
	void fill_error_response();

};

#endif //CLIENT_HPP
