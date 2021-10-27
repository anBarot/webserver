//
// Created by lubenica on 24/09/2021.
//

#ifndef WEBSERVER_REQUEST_HPP
#define WEBSERVER_REQUEST_HPP

#include "cpp_libraries.hpp"

typedef struct s_request_line
{
	e_methods method;
	std::string target;
	std::string version;

}				t_request_line;

typedef struct s_payload
{
	bool		is_chunked;
	size_t		length;
	std::string tmp_file_name;
}				t_payload;

class Request
{
	public:
		e_request_status status;
		t_request_line request_line;
		std::map<std::string, std::string> headers;
		t_payload payload;
		bool has_trailer;
		std::set<std::string> expected_trailers;

		Request() 
		{
			status = STARTING_PARSING; 
			payload.is_chunked = false;
			payload.length = 0;
			payload.tmp_file_name = "";
			has_trailer = false;
		}
};

#endif //WEBSERVER_REQUEST_HPP
