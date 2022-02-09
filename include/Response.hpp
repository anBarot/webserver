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

	Response(Request &request, Location &loc);

	std::string	get_response();

private:

	Request &req;
	e_response_code code;
	Server_conf &sv;
	Location &loc;

	std::map<std::string, std::string> headers;
	std::string file_name;
	std::string body;
	bool is_cgi;
	std::string response;

	void	create_directory_listing(std::string path, std::string loc_root);
	void	create_error(int code);

	void	method_get();
	void	method_delete();
	void	method_post();
	void	create_cgi_file();
	int		exec_cgi(const char *exec_arg);
	void	extract_cgi_file();


};

#endif //RESPONSE_HPP
