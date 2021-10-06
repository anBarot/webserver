#include "../../include/webserver.hpp"

void extract_trailer(Request &req, std::vector<char> &data)
{




}

// If transfer-encoding = chunked
void extract_in_chunks(std::string &str, std::ofstream &file, size_t pos, size_t sec_pos)
{
	size_t chunk_size;

	// get the hexadecimal chunked size
	std::stringstream sstr;
	str << str.substr(0, pos);
	str >> std::hex >> chunk_size;

	file << str.substr(0, pos + 2);
	str.erase(0, pos + 2);

	if ((pos = str.find_first_of("\r\n")) >= chunk_size)
		file << str.substr(0, chunk_size);
	else
		file << str.substr(0, pos);

	file << "\r\n";
	str.erase(0, pos + 2);
}

// If content-length header is present
void extract_with_length(std::string &str, std::ofstream &file, Request &req, std::vector<char> &data)
{
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

void extract_payload(Request &req, std::vector<char> &data)
{
	std::string str(data.begin(), data.end());

	if (req.payload.tmp_file_name == "unammed")
		req.payload.tmp_file_name = std::tmpnam(NULL);

	std::ofstream file(req.payload.tmp_file_name);

	if (req.payload.is_chunked)
	{
		size_t pos;
		size_t sec_pos;

		while ((pos = str.find_first_of("\r\n")) != std::string::npos &&
				(sec_pos = str.find_first_of("\r\n", pos + 2)) != std::string::npos)
		{
			extract_in_chunks(str, file, pos, sec_pos);
			data.erase(data.begin(), data.begin() + sec_pos + 2);
		}
	}
	else if (req.payload.length)
		extract_with_length(std::string &str, std::ofstream &file, req, data)

	file.close();
}

void extract_headers(Request &req, std::vector<char> &data)
{
	std::string str(data.begin(), data.end());
	size_t pos;
	
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

		std::string header = str.substr(0, str.find_first_of(": "));
		std::string value = str.substr(str.find_first_of(": ") + 2, str.length());
		strlower(header);
		strlower(value);

		req.headers.insert(header, value);
		str.erase(0, pos + 2);
		data.erase(data.begin(), data.begin() + pos + 2);
	}
}

void extract_request_line(Request &req, std::vector<char> &data)
{
	size_t pos;
	std::string str(data.begin(), data.end());

	if ((pos = str.find("\r\n")) != std::string::npos)
	{
		std::istringstream iss(str);
		iss >> req.request_line.method;
		iss >> req.request_line.target;
		iss >> req.request_line.version;

		data.erase(data.begin(), data.begin() + pos + 2);
		req.status = LINE_PARSED;

		req.check_line();
	}
}

int	extract_request_from_data(Client &client, std::vector<Server_conf> serv_conf)
{
	std::vector<char> data = client.received_data_raw;
	Request cur_request = client.requests.back();

	if (cur_request.status == STARTING_PARSING)
		extract_request_line(cur_request, data);
	if (cur_request.status == LINE_PARSED)
		extract_headers(cur_request, data);
	if (cur_request.status == HEADER_PARSED)
		extract_payload(cur_request, data);
	if (cur_request.status == PAYLOAD_PARSED)
		extract_trailer(cur_request, data);
	
	if (cur_request.code >= 400)
		create_response(cur_request, cur_request.code);
}
