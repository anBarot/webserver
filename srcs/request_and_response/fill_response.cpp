#include "../../include/webserver.hpp"

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
	if (i_max_matching == 0)
		return (loc_map["/"]);
	loc_path = path.substr(0, i_max_matching);
	if (loc_path.size() > 1 && loc_path[loc_path.size() - 1] == '/')
		loc_path = loc_path.substr(0, loc_path.size() - 1);
	return (loc_map[loc_path]);
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

void Client::fill_response(std::vector<Server_conf> &confs)
{
	Request &req = requests.front();
	Server_conf sv = get_server_conf(req, confs, lsocket);
	Location &loc = get_location(sv.locations, req.request_line.target);

	response.headers["Server"] = "webserver";
	response.headers["Date"] = get_date(time(NULL));
	if (response.code < 400 && loc.methods[req.request_line.method] == false) 
		response.code = METHOD_NOT_ALLOWED;
	if (response.code < 400)
	{
		if (req.request_line.method == GET)
			response.method_get(req, loc, sv);
		else if (req.request_line.method == POST)
			response.method_post(req, loc);
		else if (req.request_line.method == PUT)
			response.method_put(req, loc, sv);
		else if (req.request_line.method == DELETE)
			response.method_delete(req, loc);
	}
	if (response.code >= 400)
	{
		if (response.code == METHOD_NOT_ALLOWED)
			response.headers["Allow"] = get_allow(loc);
		if (sv.error_page.count(response.code) && get_file_size(sv.error_page[response.code]) != "")
			response.file_name = sv.error_page[response.code];
		else
			response.file_name = create_error_file(response.code);
	}
	if (response.file_name != "")
		response.headers["Content-Length"] = get_file_size(response.file_name);
	response.create_response_line();
	response.create_header_string();
	display_response(response);
	requests.pop_front();
}
