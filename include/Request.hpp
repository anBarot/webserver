//
// Created by lubenica on 24/09/2021.
//

#ifndef WEBSERVER_REQUEST_HPP
#define WEBSERVER_REQUEST_HPP

#include "cpp_libraries.hpp"

typedef struct s_request_line
{
	std::string method;
	std::string target;
	std::string version;

}				t_request_line;

class Request
{
public:

	t_request_line request_line;
	std::map<std::string, std::string> headers;
	int fd;
	std::string tmp_file_name;

	Request(){};

private:

};



#endif //WEBSERVER_REQUEST_HPP
