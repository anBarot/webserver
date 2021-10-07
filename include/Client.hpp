//
// Created by lubenica on 21/09/2021.
//

#ifndef WEBSERVER_CLIENT_HPP
#define WEBSERVER_CLIENT_HPP

#include "Request.hpp"
#include "Response.hpp"
#include "cpp_libraries.hpp"
#include "data_struct.hpp"

class Client
{
public:

	int socket;
	Response response;
	std::deque<Request> requests;
	std::vector<char> received_data_raw;

	Client(int sock): socket(sock){};

	~Client(){};

	void store_incoming_data(char buffer[BUFFER_SIZE], int size)
	{
		for (int i = 0; i < size ; i++) // probable bottleneck, will see later (should write in a temporary file ?)
			received_data_raw.push_back(buffer[i]);

		extract_request_from_data(*this);
	}
};

#endif //WEBSERVER_CLIENT_HPP
