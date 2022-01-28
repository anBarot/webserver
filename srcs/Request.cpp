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
		payload.tmp_file_name = std::tmpnam(NULL);

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
