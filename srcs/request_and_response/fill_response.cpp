#include "../../include/webserver.hpp"

void Response::create_response_line()
{
	std::stringstream sst;
	sst << "HTTP/1.1 " << (int)code << " " << reason_phrase[code] << "\r\n";
	line = sst.str();
}

void Response::create_header_string()
{
	std::stringstream sst;

	for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); it++)
		sst << it->first << ": " << it->second << "\r\n";
	sst << "\r\n";
	header_string = sst.str();
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
	if (i_mem == 0)
		return (loc_map["/"]);
	loc_path = path.substr(0, i_max_matching);
	if (loc_path[loc_path.size() - 1] == '/')
		loc_path = loc_path.substr(0, loc_path.size() - 1);
	return (loc_map[loc_path]);
}

void Client::send_response()
{
	char buf[BUFFER_SIZE];
	FILE *pFile = fopen(response.file_name.c_str(), "r");
	size_t rsize = BUFFER_SIZE;
	
	if (send(socket, response.line.c_str(), response.line.size(), 0) == -1 ||
		send(socket, response.header_string.c_str(),  response.header_string.size(), 0) == -1)
		status = 1;
	while (rsize == BUFFER_SIZE)
	{
		rsize = fread(buf, 1, BUFFER_SIZE, pFile);
		if (send(socket, buf,  rsize, 0) == -1)
			status = 1;
	}
	fclose(pFile);
}

void Client::fill_response(std::vector<Server_conf> &confs)
{
	std::cout << "fill response function : " << requests.front().code << "\n";
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
		if (req.request_line.method == GET)
			response.method_get(req, loc, sv);
		else if (req.request_line.method == POST)
			response.method_post(req, loc);
		else if (req.request_line.method == PUT)
			response.method_put(req, loc);
		else if (req.request_line.method == DELETE)
			response.method_delete(req, loc);
		else
			response.code = BAD_REQUEST;
	}
	response.create_response_line();
	response.create_header_string();
	display_response(response);
	requests.pop_front();
}
