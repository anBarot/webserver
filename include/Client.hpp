/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abarot <abarot@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/21 15:20:52 by abarot            #+#    #+#             */
/*   Updated: 2022/01/25 18:14:53 by abarot           ###   ########.fr       */
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
	std::deque<Request> requests;
	std::vector<char> received_data_raw;
	time_t last_activity;
	std::string response;

	Client(int sock, unsigned short lsock, std::string n_ip_add, time_t t);

	int		receive_request(std::vector<Server_conf> &confs);
	int		respond();

private:
	void	extract_request_from_data(std::vector<Server_conf> confs);
};

#endif //CLIENT_HPP
