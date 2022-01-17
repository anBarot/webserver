/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request_treatment.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abarot <abarot@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/21 15:32:38 by abarot            #+#    #+#             */
/*   Updated: 2022/01/17 17:44:03 by abarot           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserver.hpp"

/*
	If transfer-encoding = chunked, 
	the first line is an hexadecimal value that tells the extraction length
	of the second line
*/
void extract_in_chunks(std::string &str, std::ofstream &file, Request &req, size_t pos)
{
	size_t chunk_size;
	std::istringstream iss(str.substr(0, pos));

	iss >> std::hex >> chunk_size;
	str.erase(0, pos + 2);

	if (chunk_size == 0)
		req.status = PAYLOAD_PARSED;
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
void extract_with_length(std::string &str, std::ofstream &file, Request &req, std::vector<char> &data)
{
	if (str.size() > req.payload.length)
	{
		file.close();
		remove(req.payload.tmp_file_name.c_str());
		req.payload.tmp_file_name = "";
		data.erase(data.begin(), data.begin() + str.size());
		req.payload.length = 0;
	}
	else
	{
		file << str.substr(0, str.size());
		data.erase(data.begin(), data.begin() + str.size());
		req.payload.length -= str.size();
	}
}

/*
	If there is an expected trailers, function that extract the required headers.
	The line that contains unexpected headers or are unvalid are ignored.
*/
void extract_trailer(Request &req, std::vector<char> &data)
{
	std::string str(data.begin(), data.end());
	size_t pos;
	size_t pos_dpoint;
	std::string header;
	std::string value;

	while ((pos = str.find_first_of("\r\n")) != std::string::npos)
	{
		if ((pos_dpoint = str.find_first_of(":")) != std::string::npos)
		{
			header = str.substr(0, pos_dpoint);
			value = str.substr(str.find_first_of(": ") + 2, pos - pos_dpoint - 1);
			strlower(header);

			if (req.expected_trailers.find(header) != req.expected_trailers.end())
				req.headers[header] = value;
		}
		str.erase(0, pos + 2);
		data.erase(data.begin(), data.begin() + pos + 2);
	}
}

/* 
	Funtion that extract payload into a temporary file, with a random name. 
	The payload can be extracted by chunks or required length.
*/
void extract_payload(Request &req, std::vector<char> &data)
{
	std::string str(data.begin(), data.end());

	if (req.payload.tmp_file_name == "")
		req.payload.tmp_file_name = std::tmpnam(NULL);

	std::ofstream file(req.payload.tmp_file_name.c_str(), std::ios::out | std::ios::app);

	if (req.payload.is_chunked)
	{
		size_t pos;
		size_t sec_pos;

		while ( req.status == HEADER_PARSED &&
				(pos = str.find_first_of("\r\n")) != std::string::npos &&
				(sec_pos = str.find_first_of("\r\n", pos + 2)) != std::string::npos)
		{
			extract_in_chunks(str, file, req, pos);
			data.erase(data.begin(), data.begin() + sec_pos + 2);
		}
	}
	else
	{
		extract_with_length(str, file, req, data);
		if (!req.payload.length)
			req.status = PAYLOAD_PARSED;
	}

	if (req.status == PAYLOAD_PARSED && req.has_trailer == false)
		req.status = FINISH_PARSING;
	
	file.close();
}

/*
	Function that extract the request headers, and check if 
	there is an expected payload and trailer. 
	Headers are case insensitive and thus are put to lowercase.
*/
void extract_headers(Request &req, std::vector<char> &data)
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
			req.status = HEADER_PARSED;
			break;
		}
		pos_dpoint = str.find_first_of(": ");
		header = str.substr(0, pos_dpoint);
		value = str.substr(pos_dpoint + 2, pos - pos_dpoint - 1);
		strlower(header);

		req.headers[header] = value;
		str.erase(0, pos + 2);
		data.erase(data.begin(), data.begin() + pos + 2);
	}
}

/*
	Function that extracts the request line, then check if the method, 
	url and http version are valid.
*/
void extract_request_line(Request &req, std::vector<char> &data)
{
	size_t pos;
	std::string str(data.begin(), data.end());

	if ((pos = str.find("\r\n")) != std::string::npos)
	{
		std::istringstream iss(str);
		std::string word;

		iss >> word;
		req.request_line.method = get_method_enum(word);
		iss >> req.request_line.target;
		iss >> req.request_line.version;

		data.erase(data.begin(), data.begin() + pos + 2);
		req.status = LINE_PARSED;
	}
}

