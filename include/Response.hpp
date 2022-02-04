/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abarot <abarot@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/21 15:23:09 by abarot            #+#    #+#             */
/*   Updated: 2022/01/25 16:45:09 by abarot           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "libraries.hpp"
#include "data_struct.hpp"
#include "Location.hpp"
#include "Server_conf.hpp"
#include "webserver.hpp"

class Response
{
	public:
		e_response_code code;
		std::string response;
		std::map<std::string, std::string> headers;
		std::string file_name;
		bool is_cgi;
	
		Response() : code(OK), is_cgi(false) {}
		~Response() {}

		void clear()
		{
			code = DEFAULT;
			response.clear();
			headers.clear();
			file_name.clear();
		}

		void create_response();
		void create_directory_listing(std::string path, std::string loc_root);
		void fill_response(Server_conf &sv, Request &req, Location &loc);

		void	method_get(Request &req, Location &loc);
		void 	method_delete(Request &req, Location &loc);
		void	method_post(Request &req, Location &loc, Server_conf &sv);
		void	create_cgi_file(Request &req, Location &loc);
		int		exec_cgi(const char *exec_arg, Request &req);
		void	extract_cgi_file();
};

#endif //RESPONSE_HPP
