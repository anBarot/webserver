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
		std::vector<std::string> expected_trailers;

		Request() 
		{
			status = STARTING_PARSING; 
			code = DEFAULT;
			payload.is_chunked = false;
			payload.length = 0;
			payload.tmp_file_name = "unammed";
		}

		// check if the request line is valid
		void	check_line()
		{
			if (request_line.version != "HTTP/1.1")
				code = HTTP_VERSION_NOT_SUPPORTED;
			else if (!is_method(request_line.method))
				code = BAD_REQUEST;

			if (code != DEFAULT)
				status = FINISH_PARSING;
		}

		// check if a payload exists and is pertinent
		void	check_payload()
		{
			if (request_line.method != "GET")
			{
				if (headers.count("transfer-encoding") && headers["transfer-encoding"].find("chunked") != std::string::npos)
					payload.is_chunked = true;
				else if (headers.count("content-length"))
					payload.length = atoi(headers["content-length"].c_str());
				else
					status = FINISH_PARSING;
			}
			else
				status = FINISH_PARSING;
		}

		// check if a trailer is expected
		void	check_trailer()
		{
			if (payload.is_chunked == true && headers.count("trailer"))
			{
				std::istringstream iss(headers["trailer"]);
				std::string word;
				while (iss >> word)
					expected_trailers.push_back(word);
			}
		}
};

#endif //WEBSERVER_REQUEST_HPP
