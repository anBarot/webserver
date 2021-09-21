//
// Created by lubenica on 21/09/2021.
//

#ifndef WEBSERVER_CLIENT_HPP
#define WEBSERVER_CLIENT_HPP

class Client
{
public:

	Client(int sock): socket(sock){};
	int socket;
	std::string header;
	std::vector<char> data;
	std::string response;
};

#endif //WEBSERVER_CLIENT_HPP
