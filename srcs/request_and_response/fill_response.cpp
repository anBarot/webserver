#include "../../include/webserver.hpp"

void Response::create_response_line()
{
	std::stringstream sst;
	sst << "HTTP/1.1 " << code << " " << reason_phrase[code] << "\r\n";
	sst >> line;
}

void Response::create_header_string()
{
	std::stringstream sst;

	for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); it++)
		sst << it->first << ": " << it->second << "\r\n";
	sst << "\r\n";
	sst >> header_string;
}

Server_conf get_server_conf(Request &req, std::vector<Server_conf> &confs, int lsock)
{
	bool first_encounter = false;
	Server_conf sv;

	for (std::vector<Server_conf>::iterator conf = confs.begin(); conf != confs.end(); conf++)
	{
		if (conf->listen_port == lsock)
		{
			if (first_encounter == false)
			{
				sv = *conf;
				first_encounter = true;
			}
			else
			{
				for (std::list<std::string>::iterator sv_name = conf->names.begin(); sv_name != conf->names.end(); sv_name++)
				{
					if (*sv_name == req.headers["host"])
						return (*conf);
				}
			}
		}
	}
	return (sv);
}

Location &get_location(std::map<std::string, Location> &loc_map, std::string path)
{
	size_t i_max_matching;
	size_t i_mem;
	std::string loc_path;

	i_mem = 0;
	i_max_matching = 0;
	for (std::map<std::string, Location>::iterator it = loc_map.begin(); it != loc_map.end(); ++it)
	{
		i_mem = path.find_first_not_of(it->first);
		if (i_mem > i_max_matching)
			i_max_matching = i_mem;
	}
	loc_path = path.substr(0, i_max_matching);
	std::cout << "check req path : " << path << "\n";
	std::cout << "check loc path : " << loc_path << "\n";
	return (loc_map[loc_path]);
}

void Client::send_response()
{
	char buf[BUFFER_SIZE];
	std::ifstream file(response.file_name.c_str());
	
	if (send(socket, response.line.c_str(), response.line.size(), 0) == -1 ||
		send(socket, response.header_string.c_str(),  response.header_string.size(), 0) == -1)
		close_connection();
	while (!file.eof())
	{
		file.read(buf, BUFFER_SIZE);
		if (send(socket, buf,  BUFFER_SIZE, 0) == -1)
			close_connection();
	}
	file.close();
}

void Client::fill_response(std::vector<Server_conf> &confs)
{
	std::cout << "fill response function\n";
	Request &req = requests.front();
	Server_conf sv = get_server_conf(req, confs, lsocket);
	Location &loc = get_location(sv.locations, req.request_line.target);

	response.headers["Server"] = "webserver";
	response.headers["Date"] = get_date(time(NULL));
	if (loc.methods[req.request_line.method] == false)
	{
		response.code = METHOD_NOT_ALLOWED;
		response.headers["Allow"] = get_allow(loc);
	}
	else
	{
		switch (req.request_line.method)
		{
			case GET :
				response.method_get(req, loc);
			case POST :
				response.method_post(req, loc);
			case PUT :
				response.method_put(req, loc);
			case DELETE :
				response.method_delete(req, loc);
			case NOT_A_METHOD :
				response.code = BAD_REQUEST;
			default :
				break;
		}
	}
	if (response.code >= 400)
		response.file_name = sv.error_page[response.code];
	response.create_response_line();
	response.create_header_string();
	display_response(response);
	while (1);
}

void Client::close_connection()
{




}
