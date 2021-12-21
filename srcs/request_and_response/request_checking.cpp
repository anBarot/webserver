/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request_checking.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abarot <abarot@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/21 15:32:59 by abarot            #+#    #+#             */
/*   Updated: 2021/12/21 21:09:41 by abarot           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserver.hpp"

int	check_http_version(std::string version)
{
	size_t pos = version.find_first_of("/");
	
	if (pos == std::string::npos)
		return (1);

	std::string value_part = version.substr(pos + 1, version.size());

    if (version.substr(0, pos + 1) != "HTTP/" || std::atof(value_part.c_str()) > 1.100001
        || std::atof(value_part.c_str()) <= 0.0000000)
        return 1;

    return 0;
}

// check if the request line is valid. If not, an reponse error code is set.
void	Client::check_line()
{
	if (check_http_version(requests.back().request_line.version))
		response.code = HTTP_VERSION_NOT_SUPPORTED;
	else if (requests.back().request_line.method == NOT_A_METHOD || 
				requests.back().request_line.target[0] != '/')
		response.code = BAD_REQUEST;

	if (response.code >= 400)
		requests.back().status = FINISH_PARSING;
}

/*
	check if host header is provided, as required in HTTP/1.1 protocol
	check if a payload must be extracted and how (length or chunked).
	If not, the request status is set as finised.
*/
void	Client::check_payload()
{
	if (requests.back().request_line.method == PUT ||
		requests.back().request_line.method == POST)
	{
		if (requests.back().headers.count("transfer-encoding") &&
			requests.back().headers["transfer-encoding"].find("chunked") != std::string::npos)
			requests.back().payload.is_chunked = true;
		else if (requests.back().headers.count("content-length"))
		{
			requests.back().payload.length = atoi(requests.back().headers["content-length"].c_str());
			if (requests.back().payload.length < 0)
			{
				requests.back().status = FINISH_PARSING;
				response.code = BAD_REQUEST;
			}
		}
		else
		{
			response.code = LENGTH_REQUIRED;
			requests.back().status = FINISH_PARSING;
		}
	}
	else
		requests.back().status = FINISH_PARSING;
}

/*
	Check if a trailer is expected and header values are allowed.
	The expected header in trailer are put in the expected_trailers set.
*/
void	Client::check_trailer()
{
	std::string s[] = {"transfer-encoding", "content-length", "host", 
	"cache-control", "max-forwards", "te", "authorization", "set-cookie",
	"content-encoding", "content-type", "content-range", "trailer"};
	std::set<std::string> disallowed_trailer(s, s + 12);

	if (requests.back().payload.is_chunked == true && requests.back().headers.count("trailer"))
	{
		std::istringstream iss(requests.back().headers["trailer"]);
		std::string word;
		
		while (iss >> word)
		{
			strlower(word);
			if (disallowed_trailer.find(word) == disallowed_trailer.end())
				requests.back().expected_trailers.insert(word);
		}
	}
}
