#include "../../include/webserver.hpp"

Server_conf &get_server_conf(Request &req, std::vector<Server_conf> &confs, int lsock)
{
	bool first_encounter = false;
	Server_conf sv;

	for (std::vector<Server_conf>::iterator conf = confs.begin(); conf != confs.end(); conf++)
	{
		if (conf->get_listen_port() == lsock)
		{
			if (first_encounter == false)
			{
				sv = *conf;
				first_encounter = true;
			}
			else
			{
				for (std::list<std::string>::iterator sv_name = conf->get_name().begin(); sv_name != conf->get_name().end(); sv_name++)
				{
					if (*sv_name == req.headers["host"])
						return (*conf);
				}
			}
		}
	}
	return (sv);
}

void fill_response(Client &client, std::vector<Server_conf> &confs)
{
	Server_conf sv;
	Request &req = client.requests.front();

	while (client.requests.front().status == FINISH_PARSING)
	{
		sv = get_server_conf(req, confs, client.lsocket);
		switch (req.request_line.method)
		{
			case GET :
				response_method_get(client, client.requests.front(), sv);
			case POST :
				response_method_post();
			case PUT :
				response_method_put();
			case DELETE :
				response_method_delete();
			case NOT_A_METHOD :
				client.requests.front().code = 500;
			default :
				break;
		}
	}
}
