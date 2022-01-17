/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abarot <abarot@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/21 15:23:00 by abarot            #+#    #+#             */
/*   Updated: 2022/01/17 17:42:36 by abarot           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "libraries.hpp"
#include "data_struct.hpp"

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

#endif //REQUEST_HPP
