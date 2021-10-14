#include "../../include/webserver.hpp"

void create_response_line(e_response_code code, std::string &line)
{
	std::stringstream sst;
	sst << "HTTP/1.1 " << code << " " << reason_phrase[code] << "\r\n";
	sst >> line;
}

void create_header_allow(std::map<std::string, std::string> &headers, Location &loc)
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

void fill_response(Client &client, std::vector<Server_conf> &confs)
{
	Server_conf sv;
	Location loc;
	Request &req = client.requests.front();

	client.response.headers["Server"] = "webserver";
	while (client.requests.front().status == FINISH_PARSING)
	{
		sv = get_server_conf(req, confs, client.lsocket);
		loc = get_location(sv.locations, req.request_line.target);
		client.response.headers["Date"] = get_date(time(NULL));
		if (loc.methods[req.request_line.method] == false)
		{
			req.code = METHOD_NOT_ALLOWED;
			create_header_allow(client.response.headers, loc);
		}
		switch (req.request_line.method)
		{
			case GET :
				response_method_get(client, client.requests.front(), sv, loc);
			case POST :
				response_method_post();
			case PUT :
				response_method_put();
			case DELETE :
				response_method_delete();
			case NOT_A_METHOD :
				client.requests.front().code = BAD_REQUEST;
			default :
				break;
		}
		create_response_line(req.code, client.response.line);
	}
}
