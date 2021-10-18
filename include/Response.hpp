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

		void create_response_line();
		void create_header_string();
		void get_index_file(std::string &path);
		void create_directory_listing(std::string &path);
		void method_get(Request &req, Location &loc, Server_conf &sv);
		void method_delete(Request &req, Location &loc);
		void method_put(Request &req, Location &loc);
		void method_post(Request &req, Location &loc);
};

#endif //WEBSERVER_RESPONSE_HPP
