#include "Client.hpp"

Client::Client(int sock, unsigned short lsock, std::string n_ip_add, time_t t) : socket(sock), ip_address(n_ip_add), port(lsock), last_activity(t)
{
	requests.push_back(Request());
}

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
	Server_conf sv;
	size_t pos;

	pos = sv_name.find_first_of(":");
	if (pos != std::string::npos)
		sv_name.erase(pos);
	for (std::vector<Server_conf>::iterator conf = confs.begin(); conf != confs.end(); ++conf)
	{
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

int Client::receive_request(std::vector<Server_conf> &confs)
{
	char buffer[BUFFER_SIZE];
	ssize_t ret;

	ret = recv(socket, buffer, BUFFER_SIZE - 1, 0);
	if (ret <= 0 || has_telnet_breaksignal(ret, buffer))
		return -1;
	buffer[ret] = 0;
	std::cout << socket << " received " << ret << " bytes:\n" << buffer << std::endl;

	for (int i = 0; buffer[i]; ++i)
		received_data_raw.push_back(buffer[i]);
	if (received_data_raw.size())
	{
		if (requests.size() && requests.back().status != FINISH_PARSING)
			extract_request_from_data(confs);
		else
			requests.push_back(Request());
	}
	return 0;
}

/*
	When receiving raw data, this function allow extraction
	depending on the request status :
	starting -> line parsed -> header parsed -> payload parsed -> finish
*/
void Client::extract_request_from_data(std::vector<Server_conf> confs)
{
	Request &req = requests.back();

	if (req.status == STARTING_PARSING)
	{
		if (received_data_raw[0] == '\r' && received_data_raw[1] == '\n')
		{
			received_data_raw.erase(received_data_raw.begin(), received_data_raw.begin() + 2);
			return ;
		}
		req.extract_request_line(received_data_raw);
		if (req.status == LINE_PARSED) 
			check_line();
	}
	if (req.status == LINE_PARSED)
	{
		req.extract_headers(received_data_raw);
		if (req.status == HEADER_PARSED)
			check_payload(confs);
	}
	if (req.status == HEADER_PARSED)
		req.extract_payload(received_data_raw);
}

// check if the request line is valid. If not, an reponse error code is set.
void	Client::check_line()
{
	if (check_http_version(requests.back().request_line.version))
		response.code = HTTP_VERSION_NOT_SUPPORTED;
	else if (requests.back().request_line.method == NOT_A_METHOD)
		response.code = NOT_IMPLEMENTED;
	else if	(requests.back().request_line.target[0] != '/')
		response.code = BAD_REQUEST;
	else if (requests.back().request_line.target.size() >= 256)
		response.code = URI_TOO_LONG;

	if (response.code >= 400)
		requests.back().status = FINISH_PARSING;
}

/*
	check if host header is provided, as required in HTTP/1.1 protocol
	check if a payload must be extracted and how (length or chunked).
	If not, the request status is set as finised.
*/
void	Client::check_payload(std::vector<Server_conf> confs)
{
	Request &req = requests.back();

	if (req.headers.size() > 32 || req.headers.count("host") == 0)
	{
		req.status = FINISH_PARSING;
		response.code = BAD_REQUEST;
		return ;
	}

	for (std::map<std::string, std::string>::iterator it = req.headers.begin(); 
	it != req.headers.end(); it++)
	{
		if (it->first.find_first_of("\t\r ") != std::string::npos)
		{
			req.status = FINISH_PARSING;
			response.code = BAD_REQUEST;
			return ;
		}
	}

	Server_conf sv = get_server_conf(confs, port, ip_address, req.headers["host"]);

	if (req.request_line.method == POST)
	{
		if (req.headers.count("transfer-encoding") &&
			req.headers["transfer-encoding"] == "chunked")
			req.payload.is_chunked = true;
		else if (req.headers.count("transfer-encoding"))
		{
			req.status = FINISH_PARSING;
			response.code = NOT_IMPLEMENTED;
		}
		else if (req.headers.count("content-length"))
		{
			req.payload.length = atoi(req.headers["content-length"].c_str());
			if (req.payload.length < 0)
			{
				req.status = FINISH_PARSING;
				response.code = BAD_REQUEST;
			}
			else if (req.payload.length > sv.max_body_size)
			{
				req.status = FINISH_PARSING;
				response.code = PAYLOAD_TOO_LARGE;
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

int Client::respond(std::vector<Server_conf> &confs)
{
	std::stringstream buf;
	std::string str;
	std::ifstream file;

	fill_response(confs);
	file.open(response.file_name.c_str());
	buf << response.line.c_str() << response.header_string.c_str() << file.rdbuf() << "\r\n";
	str = buf.str();
	// std::cout << "Response:\n" << str << std::endl;
	response.clear();
	file.close();
	if (send(socket, str.c_str(), str.size(), 0) == -1)
		return -1;
	// std::cout << "Response sent to " << socket << std::endl;
	return 0;
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
		else if (req.request_line.method == POST)
			response.method_post(req, loc, sv);
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
