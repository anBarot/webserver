//
// Created by lubenica on 26/09/2021.
//

#ifndef WEBSERVER_RESPONSE_HPP
#define WEBSERVER_RESPONSE_HPP

#include "cpp_libraries.hpp"
#include "data_struct.hpp"

class Response
{
	public:
		std::string status_line;
		std::map<std::string, std::string> headers;
		std::vector<char> payload;
};

#endif //WEBSERVER_RESPONSE_HPP
