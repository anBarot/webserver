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
		e_response_code code;
		std::string line;
		std::string header_string;
		std::map<std::string, std::string> headers;
		std::string file_name;

		void create_response_line()
		{
			std::stringstream sst;
			sst << "HTTP/1.1 " << code << " " << reason_phrase[code] << "\r\n";
			sst >> line;
		}

		void Response::create_header_string()
		{
			std::stringstream sst;

			for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); it++)
				sst << it->first << ": " << it->second << "\r\n";
			sst << "\r\n";
			sst >> header_string;
		}

		void get_index_file(std::string &path);
		void create_directory_listing(std::string &path);
		void method_get(Request &req, Location &loc);
		void method_delete(Request &req, Location &loc);
		void method_put(Request &req, Location &loc);
		void method_post(Request &req, Location &loc);
};

#endif //WEBSERVER_RESPONSE_HPP
