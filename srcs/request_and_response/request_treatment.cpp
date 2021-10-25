#include "../../include/webserver.hpp"

/*
	If transfer-encoding = chunked, 
	the first line is an hexadecimal value that tells the extraction length
	of the second line
*/
void extract_in_chunks(std::string &str, std::ofstream &file, Request &req, size_t pos, size_t sec_pos)
{
	size_t chunk_size;
	
	std::stringstream sstr;
	sstr << str.substr(0, pos);
	sstr >> std::hex >> chunk_size;

	file << str.substr(0, pos + 2);
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
	std::cout << "payload length : " << req.payload.length << "\n";
	std::cout << "string : " << str << "\n";
	std::cout << "file name : " << req.payload.tmp_file_name << "\n";
	if (str.size() >= req.payload.length)
	{
		file << str.substr(0, req.payload.length);
		data.erase(data.begin(), data.begin() + req.payload.length);
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
	std::string header;
	std::string value;

	while ((pos = str.find_first_of("\r\n")) != std::string::npos)
	{
		if (str.find_first_of(":") != std::string::npos)
		{
			header = str.substr(0, str.find_first_of(": "));
			value = str.substr(str.find_first_of(": ") + 2, str.length());
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

	std::ofstream file(req.payload.tmp_file_name.c_str());

	if (req.payload.is_chunked)
	{
		size_t pos;
		size_t sec_pos;

		while ( req.status == HEADER_PARSED &&
				(pos = str.find_first_of("\r\n")) != std::string::npos &&
				(sec_pos = str.find_first_of("\r\n", pos + 2)) != std::string::npos)
		{
			extract_in_chunks(str, file, req, pos, sec_pos);
			data.erase(data.begin(), data.begin() + sec_pos + 2);
		}
	}
	else if (req.payload.length)
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
	std::string header;
	std::string value;
	
	while ((pos = str.find_first_of("\r\n")) != std::string::npos)
	{
		if (str.find_first_of("\r\n") == 0)
		{
			data.erase(data.begin(), data.begin() + 2);

			req.status = HEADER_PARSED;
			req.check_payload();
			req.check_trailer();
			break;
		}

		header = str.substr(0, str.find_first_of(": "));
		value = str.substr(str.find_first_of(": ") + 2, str.length());
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

		req.check_line();
	}
}

/*
	When receiving raw data, this function allow extraction
	depending on the request status :
	starting -> line parsed -> header parsed -> payload parsed -> finish
*/
void extract_request_from_data(Request &cur_request, std::vector<char> &data)
{
	if (cur_request.status == STARTING_PARSING)
		extract_request_line(cur_request, data);
	if (cur_request.status == LINE_PARSED)
		extract_headers(cur_request, data);
	if (cur_request.status == HEADER_PARSED)
		extract_payload(cur_request, data);
	if (cur_request.status == PAYLOAD_PARSED)
		extract_trailer(cur_request, data);
	
	display_request(cur_request);
}

void Client::store_incoming_data(char *buffer, int size)
{
	for (int i = 0; i < size ; i++)
		received_data_raw.push_back(buffer[i]);

	while (received_data_raw.size())
	{
		if (requests.size() && requests.back().status != FINISH_PARSING)
			extract_request_from_data(this->requests.back(), this->received_data_raw);
		else
			requests.push_back(Request());
	}
}
