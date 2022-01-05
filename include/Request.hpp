/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abarot <abarot@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/21 15:23:00 by abarot            #+#    #+#             */
/*   Updated: 2022/01/05 17:05:24 by abarot           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "libraries.hpp"

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
	size_t		length_restriction;
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

		Request(size_t max_body_size)
		{
			status = STARTING_PARSING; 
			payload.is_chunked = false;
			payload.length = 0;
			payload.length_restriction = max_body_size;
			payload.tmp_file_name = "";
			has_trailer = false;
		}
};

#endif //REQUEST_HPP
