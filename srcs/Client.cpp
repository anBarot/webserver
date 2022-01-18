#include "Client.hpp"

Location &get_location(std::map<std::string, Location> &loc_map, std::string path)
{
	size_t i_max_matching;
	size_t i_mem;
	std::string loc_path;
	std::string subpath;

	i_mem = 0;
	i_max_matching = 0;
	for (std::map<std::string, Location>::iterator it = loc_map.begin(); it != loc_map.end(); ++it)
	{
		subpath = path.substr(0, it->first.size());
		(subpath == it->first) ? i_mem = subpath.size() : i_mem = 0;
		if (i_mem > i_max_matching)
			i_max_matching = i_mem;
	}
	if (i_max_matching == 0)
		return (loc_map["/"]);
	loc_path = path.substr(0, i_max_matching);
	if (loc_path.size() > 1 && loc_path[loc_path.size() - 1] == '/')
		loc_path = loc_path.substr(0, loc_path.size() - 1);
	return (loc_map[loc_path]);
}

Server_conf get_server_conf(std::vector<Server_conf> &confs, unsigned short port, std::string ip, std::string sv_name)
{
	bool first_encounter = false;
	std::string host_name;
	Server_conf sv;

	for (std::vector<Server_conf>::iterator conf = confs.begin(); conf != confs.end(); conf++)
	{
		std::cout << "check conf listen port : " << conf->listen_port << "\n";
		std::cout << "check browser listen port : " << port << "\n";
		std::cout << "check conf listen ip : " << conf->listen_ip << "\n";
		std::cout << "check browser listen ip : " << ip << "\n";
		std::cout << "check browser sv name : " << sv_name << "\n";

		if (conf->listen_port == port && conf->listen_ip == ip)
		{
			if (first_encounter == false)
			{
				sv = *conf;
				first_encounter = true;
			}
			else
			{
				for (std::list<std::string>::iterator it = conf->names.begin(); it != conf->names.end(); it++)
				{
					if (*it == sv_name)
					{
						sv = *conf;
						break;
					}
				}
			}
		}
	}
	return (sv);
}

std::string	create_error_file(int code)
{
	std::ifstream filein("./html/error.html");
	std::ofstream fileout("./error_temp.html");
	size_t pos_done;
	size_t pos_dtwo;
	std::string str;

	std::ostringstream num;
	num << code;
 	
	while (std::getline(filein, str))
	{
		if ((pos_done = str.find("$1")) != std::string::npos)
			str.replace(pos_done, 2, num.str());
		if ((pos_dtwo = str.find("$2")) != std::string::npos)
			str.replace(pos_dtwo, 2, reason_phrase[code]);
		fileout << str;
		fileout << "\n";
	}
	filein.close();
	fileout.close();
	str = "./error_temp.html";
	return str;
}

int	check_http_version(std::string version)
{
	size_t pos = version.find_first_of("/");
	
	if (pos == std::string::npos)
		return (1);

	std::string value_part = version.substr(pos + 3, version.size());

    if (value_part.find_first_not_of("0123456789") != std::string::npos
        || version.substr(0, pos + 3) != "HTTP/1.")
        return 1;

    return 0;
}


void Client::store_incoming_data(char *buffer, int size)
{
	for (int i = 0; i < size ; i++)
		received_data_raw.push_back(buffer[i]);

	if (received_data_raw.size())
	{
		if (requests.size() && requests.back().status != FINISH_PARSING)
			extract_request_from_data(received_data_raw);
		else
			requests.push_back(Request());
	}
}

/*
	When receiving raw data, this function allow extraction
	depending on the request status :
	starting -> line parsed -> header parsed -> payload parsed -> finish
*/
void Client::extract_request_from_data(std::vector<char> &data)
{
	if (requests.back().status == STARTING_PARSING)
	{
		if (data[0] == '\r' && data[1] == '\n')
		{
			data.erase(data.begin(), data.begin() + 2);
			return ;
		}
		requests.back().extract_request_line(data);
		if (requests.back().status == LINE_PARSED) 
			check_line();
	}
	if (requests.back().status == LINE_PARSED)
	{
		requests.back().extract_headers(data);
		if (requests.back().status == HEADER_PARSED)
		{
			check_payload();
			check_trailer();
		}
	}
	if (requests.back().status == HEADER_PARSED)
		requests.back().extract_payload(data);
	if (requests.back().status == PAYLOAD_PARSED)
		requests.back().extract_trailer(data);
}

void Client::fill_response(std::vector<Server_conf> confs)
{
	Request &req = requests.front();
	Server_conf sv = get_server_conf(confs, port, ip_address, req.headers["host"]);
	Location &loc = get_location(sv.locations, req.request_line.target);

	response.headers["Server"] = "webserver";
	response.headers["Date"] = get_date(time(NULL));
	if (loc.redirection.first >= 300)
	{
		response.code = loc.redirection.first;
		response.file_name = sv.error_page[response.code];
		response.headers["Location"] = loc.redirection.second;
	}
	if (response.code < 300 && loc.methods[req.request_line.method] == false) 
		response.code = METHOD_NOT_ALLOWED;
	if (response.code < 300)
	{
		if (is_cgi_compatible(req, loc))
		{
			response.is_cgi = true;
			response.create_cgi_file(req, loc);
			response.extract_cgi_file();
		}
		else if (req.request_line.method == GET)
			response.method_get(req, loc);
		else if (req.request_line.method == PUT || req.request_line.method == POST)
			response.method_put(req, loc, sv);
		else if (req.request_line.method == DELETE)
			response.method_delete(req, loc);
	}
	if (response.code >= 400)
	{
		if (response.code == METHOD_NOT_ALLOWED)
			response.headers["Allow"] = get_allow(loc);
		if (sv.error_page.count(response.code))
			response.file_name = sv.error_page[response.code];
		else
			response.file_name = create_error_file(response.code);
	}
	if (response.file_name != "")
		response.headers["Content-Length"] = get_file_size(response.file_name);
	response.create_response_line();
	response.create_header_string();
	requests.pop_front();
}

void Client::send_response()
{
	std::ifstream file(response.file_name.c_str());
	std::stringstream buf;
	std::string str;

	if (send(socket, response.line.c_str(), response.line.size(), 0) == -1 ||
		send(socket, response.header_string.c_str(),  response.header_string.size(), 0) == -1)
		status = 1;
	buf << file.rdbuf();
	str = buf.str();
	if (send(socket, str.c_str(),  str.size(), 0) == -1 ||
		send(socket, "\r\n", 2, 0) == -1)
		status = 1;
	file.close();
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
	Request &req = requests.back();

	if (req.request_line.method == PUT ||
		req.request_line.method == POST)
	{
		if (req.headers.count("transfer-encoding") &&
			req.headers["transfer-encoding"].find("chunked") != std::string::npos)
			req.payload.is_chunked = true;
		else if (req.headers.count("content-length"))
		{
			req.payload.length = atoi(req.headers["content-length"].c_str());
			if (req.payload.length < 0)
			{
				req.status = FINISH_PARSING;
				response.code = BAD_REQUEST;
			}
		}
		else
		{
			response.code = LENGTH_REQUIRED;
			req.status = FINISH_PARSING;
		}
	}
	else
		req.status = FINISH_PARSING;
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