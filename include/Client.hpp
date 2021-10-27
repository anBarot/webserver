//
// Created by lubenica on 21/09/2021.
//

#ifndef WEBSERVER_CLIENT_HPP
#define WEBSERVER_CLIENT_HPP

#include "Request.hpp"
#include "Response.hpp"
#include "Location.hpp"
#include "Server_conf.hpp"
#include "cpp_libraries.hpp"

class Client
{
public:
	int socket;
	int lsocket;
	int status;
	Response response;
	std::deque<Request> requests;
	std::vector<char> received_data_raw;

	Client(int sock, int lsock): socket(sock), lsocket(lsock), status(0)
	{
		requests.push_back(Request());
	}

	~Client(){};

	void store_incoming_data(char buffer[BUFFER_SIZE], int size);
	void extract_request_from_data(std::vector<char> &data);
	void check_payload();
	void check_trailer();
	void check_line();
	void fill_response(std::vector<Server_conf> &confs);
	void fill_error_response();
	void send_response();
};

#endif //WEBSERVER_CLIENT_HPP
