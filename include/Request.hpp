/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abarot <abarot@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/21 15:23:00 by abarot            #+#    #+#             */
/*   Updated: 2022/01/25 18:29:06 by abarot           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "libraries.hpp"
#include "data_struct.hpp"
#include "webserver.hpp"
#include "Server_conf.hpp"
#include "Response.hpp"

class Response;

typedef struct s_request_line
{
	e_methods	method;
	std::string target;
	std::string version;
}				t_request_line;

typedef struct s_payload
{
	bool		is_chunked;
	size_t		length;
}				t_payload;

class Request
{
public:
	e_request_status status;
	t_request_line request_line;
	std::map<std::string, std::string> headers;
	t_payload payload;
	Server_conf	sv;
	e_response_code response_code;
	std::string body;

	Request();

	void	extract_request_line(std::vector<char> &data);
	void 	extract_headers(std::vector<char> &data);
	void	extract_payload(std::vector<char> &data);
	void 	extract_with_length(std::string &str, std::vector<char> &data);
	void 	extract_in_chunks(std::string &str, size_t pos);

	void	check_line();
	void 	check_payload();
	
	int		is_cgi_compatible(Location &loc);
	void	set_environment();
	
};

#endif //REQUEST_HPP
