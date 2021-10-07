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
		e_response_code code;
		t_request_line request_line;
		std::map<std::string, std::string> headers;
		t_payload payload;
		std::set<std::string> expected_trailers;

		Request() 
		{
			status = STARTING_PARSING; 
			code = DEFAULT;
			payload.is_chunked = false;
			payload.length = 0;
			payload.tmp_file_name = "unammed";
		}
};

int	extract_request_from_data(Client &client);
void check_payload(Request &req);
void check_trailer(Request &req);
void check_line(Request &req);

#endif //WEBSERVER_REQUEST_HPP
