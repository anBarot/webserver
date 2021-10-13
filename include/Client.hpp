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
	int lsocket;
	Response response;
	std::deque<Request> requests;
	std::vector<char> received_data_raw;

	Client(int sock, int lsock): socket(sock), lsocket(lsock) {}

	~Client(){};

	void store_incoming_data(char buffer[BUFFER_SIZE], int size)
	{
		for (int i = 0; i < size ; i++) // probable bottleneck, will see later (should write in a temporary file ?)
			received_data_raw.push_back(buffer[i]);

		while (received_data_raw.size())
		{
			if (requests.size() && requests.back().status != FINISH_PARSING)
				extract_request_from_data(this->requests.back(), this->received_data_raw);
			else
				requests.push_back(Request());
		}
	}
};

#endif //WEBSERVER_CLIENT_HPP
