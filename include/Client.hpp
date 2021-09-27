//
// Created by lubenica on 21/09/2021.
//

#ifndef WEBSERVER_CLIENT_HPP
#define WEBSERVER_CLIENT_HPP

#include "Request.hpp"
#include "cpp_libraries.hpp"

class Client
{
	public:
		Client(int sock): socket(sock){};
		int socket;
		std::queue<Request> requests;
		std::vector<char> buffer;
};

#endif //WEBSERVER_CLIENT_HPP
