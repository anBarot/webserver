#include "../../include/webserver.hpp"

void Response::create_header_allow(Location &loc)
{
	std::stringstream sst;

	if (loc.methods[GET] == true)
		sst << "GET ";
	if (loc.methods[POST] == true)
		sst << "POST ";
	if (loc.methods[PUT] == true)
		sst << "PUT ";
	if (loc.methods[DELETE] == true)
		sst << "DELETE";
	sst << "\r\n";
	sst >> headers["Allow"];
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

Location get_location(std::map<std::string, Location> &loc_map, std::string path)
{
	size_t i_max_matching;
	size_t i_mem; 

	i_mem = 0;
	i_max_matching = 0;
	for (std::map<std::string, Location>::iterator it = loc_map.begin(); it != loc_map.end(); it++)
	{
		i_mem = path.find_first_not_of(it->first);
		if (i_mem > i_max_matching)
			i_max_matching = i_mem;
	}
	return (loc_map[path.substr(0, i_max_matching)]);
}

void Client::send_response()
{
	char buf[BUFFER_SIZE];
	std::ifstream file (response.file_name);
	
	if (send(socket, response.line.c_str(), response.line.size(), NULL) == -1 ||
		send(socket, response.header_string.c_str(),  response.header_string.size(), NULL) == -1)
		close_connection();
	while (!file.eof())
	{
		file.read(buf, BUFFER_SIZE);
		if (send(socket, buf,  BUFFER_SIZE, NULL) == -1)
			close_connection();
	}
	file.close();
}

void Client::fill_response(std::vector<Server_conf> &confs)
{
	Request &req = requests.front();
	Server_conf sv = get_server_conf(req, confs, lsocket);
	Location loc = get_location(sv.locations, req.request_line.target);

	response.headers["Server"] = "webserver";
	response.headers["Date"] = get_date(time(NULL));
	if (loc.methods[req.request_line.method] == false)
	{
		response.code = METHOD_NOT_ALLOWED;
		response.create_header_allow(loc);
	}
	else
	{
		switch (req.request_line.method)
		{
			case GET :
				response.method_get(req, loc);
			case POST :
				response.method_post();
			case PUT :
				response.method_put();
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
}

void Client::close_connection()
{
	



}
