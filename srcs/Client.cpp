#include "Client.hpp"

Client::Client(int sock, unsigned short lsock, std::string n_ip_add, time_t t) :
		socket(sock), ip_address(n_ip_add), port(lsock),
		requests(std::deque<Request>(1)), last_activity(t)
{}

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
		for (Server_conf::listenables::iterator l = conf->listens.begin(); l != conf->listens.end(); ++l)
			if (l->port == port && l->address == ip)
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
	return (sv);
}

int Client::receive_request(std::vector<Server_conf> &confs)
{
	char buffer[BUFFER_SIZE];
	ssize_t ret;

	ret = recv(socket, buffer, BUFFER_SIZE - 1, 0);
	if (ret <= 0 || has_telnet_breaksignal(ret, buffer)) // verify both -1 and 0
		return -1;
	buffer[ret] = 0;

	#ifdef LOGGER
		std::cout << CYAN << socket << " received " << ret << " bytes:\n" << RESET << buffer << RESET << std::endl;
	#endif // DEBUG

	for (int i = 0; buffer[i]; ++i)
		received_data_raw.push_back(buffer[i]);
	if (received_data_raw.size())
	{
		if (requests.size())
		{
			Request &req(requests.back());

			if (req.status != FINISH_PARSING)
				extract_request_from_data(confs);
			if (req.status == FINISH_PARSING)
				requests.push_back(Request());
		}
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
			req.check_line();
	}
	if (req.status == LINE_PARSED)
	{
		req.extract_headers(received_data_raw);
		if (req.status == HEADER_PARSED)
		{
			req.sv = get_server_conf(confs, port, ip_address, req.headers["host"]);
			req.check_payload();
		}
	}
	if (req.status == HEADER_PARSED)
		req.extract_payload(received_data_raw);
}

int Client::respond()
{
	Request &req = requests.front();
	Location &loc = get_location(req.sv.locations, req.request_line.target);
	Response resp(req, loc);
	std::string response;
	size_t ret;

	response = resp.get_response();
	requests.pop_front();
	
	#ifdef LOGGER
		std::cout << RED << "Response:\n" << RESET << response << std::endl;
	#endif // DEBUG

	ret = send(socket, response.c_str(), response.size(), 0);
	
	#ifdef LOGGER
		std::cout << CYAN << socket << " sent " << ret << " bytes out of " << response.size() << ".\n" << RESET << std::endl;
	#endif // DEBUG

	if (ret < 0)
		return -1;
	else if (ret < response.size())
		return 1;

	return 0;
}