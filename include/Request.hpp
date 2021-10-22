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
			payload.tmp_file_name = "";
		}

		void check_payload();
		void check_trailer();
		void check_line();
		void check_request_error(std::vector<Server_conf> &server_conf);
};

void extract_request_from_data(Request &cur_request, std::vector<char> &data);

#endif //WEBSERVER_REQUEST_HPP
