//
// Created by lubenica on 21/09/2021.
//

#ifndef WEBSERVER_CLIENT_HPP
#define WEBSERVER_CLIENT_HPP

#include "Request.hpp"
#include "cpp_libraries.hpp"
#include "data_struct.hpp"

class Client
{
public:

	int socket;
	std::queue<Request> requests;
	std::vector<char> received_data_raw;


	Client(int sock): socket(sock){};

	~Client(){};

	void store_incoming_data(char buffer[BUFFER_SIZE], int size)
	{
		for (int i = 0; i < size ; i++) // probable bottleneck, will see later
			received_data_raw.push_back(buffer[i]);
	}

	void extract_request_from_data()
	{
		// if a complete request is extractable from the raw data,
		//		create a request object in the <queue> and remove the data from the <vector><char>
	}


};

#endif //WEBSERVER_CLIENT_HPP
