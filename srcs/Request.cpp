/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adda-sil <adda-sil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/21 15:32:38 by abarot            #+#    #+#             */
/*   Updated: 2022/01/28 05:12:35 by adda-sil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

Request::Request() : status(STARTING_PARSING)
{
	payload.is_chunked = false;
	payload.length = 0;
	payload.tmp_file_name = "";
}


/*
	Function that extracts the request line, then check if the method, 
	url and http version are valid.
*/
void Request::extract_request_line(std::vector<char> &data)
{
	size_t pos;
	std::string str(data.begin(), data.end());

	if ((pos = str.find("\r\n")) != std::string::npos)
	{
		std::istringstream iss(str);
		std::string word;

		iss >> word;
		request_line.method = get_method_enum(word);
		iss >> request_line.target;
		iss >> request_line.version;

		data.erase(data.begin(), data.begin() + pos + 2);
		status = LINE_PARSED;
	}
}

/*
	Function that extract the request headers, and check if 
	there is an expected payload. 
	Headers are case insensitive and thus are put to lowercase.
*/
void Request::extract_headers(std::vector<char> &data)
{
	std::string str(data.begin(), data.end());
	size_t pos;
	size_t pos_dpoint;
	std::string header;
	std::string value;
	
	while ((pos = str.find("\r\n")) != std::string::npos)
	{
		if (pos == 0)
		{
			data.erase(data.begin(), data.begin() + 2);
			status = HEADER_PARSED;
			break;
		}
		pos_dpoint = str.find_first_of(":");
		if (pos_dpoint != 0)
		{
			header = str.substr(0, pos_dpoint);
			value = str.substr(pos_dpoint + 2, pos - pos_dpoint - 2);
			strlower(header);
			headers[header] = value;
		}

		str.erase(0, pos + 2);
		data.erase(data.begin(), data.begin() + pos + 2);
	}
}

/* 
	Function that extract payload into a temporary file, with a random name. 
	The payload can be extracted by chunks or required length.
*/
void Request::extract_payload(std::vector<char> &data)
{
	std::string str(data.begin(), data.end());
	if (payload.tmp_file_name == "")
		payload.tmp_file_name = random_filename();
	std::ofstream file(payload.tmp_file_name.c_str(), std::ios::out | std::ios::app);

	if (payload.is_chunked)
	{
		size_t pos;
		size_t sec_pos;

		while ( status == HEADER_PARSED &&
				(pos = str.find_first_of("\r\n")) != std::string::npos &&
				(sec_pos = str.find_first_of("\r\n", pos + 2)) != std::string::npos)
		{
			extract_in_chunks(str, file, pos);
			data.erase(data.begin(), data.begin() + sec_pos + 2);
		}
	}
	else
	{
		extract_with_length(str, file, data);
		if (!payload.length)
			status = PAYLOAD_PARSED;
	}

	if (status == PAYLOAD_PARSED)
		status = FINISH_PARSING;
	
	file.close();
}

/*
	If transfer-encoding = chunked, 
	the first line is an hexadecimal value that tells the extraction length
	of the second line
*/
void Request::extract_in_chunks(std::string &str, std::ofstream &file, size_t pos)
{
	size_t chunk_size;
	std::istringstream iss(str.substr(0, pos));

	iss >> std::hex >> chunk_size;
	str.erase(0, pos + 2);

	if (chunk_size == 0)
		status = PAYLOAD_PARSED;
	else if ((pos = str.find_first_of("\r\n")) >= chunk_size)
		file << str.substr(0, chunk_size);
	else
		file << str.substr(0, pos);

	file << "\r\n";
	str.erase(0, pos + 2);
}

/* 
	If content-length header is present and > 0, extract by length. 
	If the length of the raw extracted data is greater than the requested length,
	extract only the length.
*/
void Request::extract_with_length(std::string &str, std::ofstream &file, std::vector<char> &data)
{
	if (str.size() > payload.length)
	{
		file.close();
		remove(payload.tmp_file_name.c_str());
		payload.tmp_file_name = "";
		data.erase(data.begin(), data.begin() + str.size());
		payload.length = 0;
	}
	else
	{
		file << str.substr(0, str.size());
		data.erase(data.begin(), data.begin() + str.size());
		payload.length -= str.size();
	}
}

int	Request::is_cgi_compatible(Location &loc)
{
	std::string &target = request_line.target;
	std::string ext;
	size_t pos;

	if (loc.cgi_path == "" || request_line.method == DELETE || 
		((pos = target.find_last_of(".")) == std::string::npos))
		return 0;
	ext = target.substr(pos + 1, target.size());
	if (std::find(loc.cgi_extension.begin(), loc.cgi_extension.end(), ext) != loc.cgi_extension.end())
		return 1; 
	return 0;
}

void Request::set_environment()
{
	std::string &req_path = request_line.target;
	std::string script_name = req_path.substr(req_path.find_last_of("/") + 1, req_path.size());

	setenv("CONTENT_TYPE", headers["content-type"].c_str(), 1);
	setenv("CONTENT_LENGTH", headers["content-length"].c_str(), 1);
	setenv("PATH_INFO", req_path.c_str(), 1);
	setenv("QUERY_STRING", get_query(payload.tmp_file_name).c_str(), 1);
	setenv("REQUEST_METHOD", get_method_string(request_line.method).c_str(), 1);
	setenv("SCRIPT_NAME", script_name.c_str(), 1);
	setenv("SCRIPT_FILENAME", req_path.c_str(), 1);
	setenv("SERVER_NAME", std::string("webserver").c_str(), 1);
}

// check if the request line is valid. If not, an reponse error code is set.
void	Request::check_line()
{
	if (check_http_version(request_line.version))
		response_code = HTTP_VERSION_NOT_SUPPORTED;
	else if (request_line.method == NOT_A_METHOD)
		response_code = NOT_IMPLEMENTED;
	else if	(request_line.target[0] != '/')
		response_code = BAD_REQUEST;
	else if (request_line.target.size() >= 256)
		response_code = URI_TOO_LONG;

	if (response_code >= 400)
		status = FINISH_PARSING;
}

/*
	check if host header is provided, as required in HTTP/1.1 protocol
	check if a payload must be extracted and how (length or chunked).
	If not, the request status is set as finised.
*/
void	Request::check_payload()
{
	if (headers.size() > 32 || headers.count("host") == 0)
	{
		status = FINISH_PARSING;
		response_code = BAD_REQUEST;
		return ;
	}

	for (std::map<std::string, std::string>::iterator it = headers.begin(); 
	it != headers.end(); it++)
	{
		if (it->first.find_first_of("\t\r ") != std::string::npos)
		{
			status = FINISH_PARSING;
			response_code = BAD_REQUEST;
			return ;
		}
	}

	if (request_line.method == POST)
	{
		if (headers.count("transfer-encoding") &&
			headers["transfer-encoding"] == "chunked")
			payload.is_chunked = true;
		else if (headers.count("transfer-encoding"))
		{
			status = FINISH_PARSING;
			response_code = NOT_IMPLEMENTED;
		}
		else if (headers.count("content-length"))
		{
			payload.length = atoi(headers["content-length"].c_str());
			if (payload.length < 0)
			{
				status = FINISH_PARSING;
				response_code = BAD_REQUEST;
			}
			else if (payload.length > sv.max_body_size)
			{
				status = FINISH_PARSING;
				response_code = PAYLOAD_TOO_LARGE;
			}
		}
		else
		{
			response_code = LENGTH_REQUIRED;
			status = FINISH_PARSING;
		}
	}
	else
		status = FINISH_PARSING;
}
