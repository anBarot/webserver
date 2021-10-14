//
// Created by lubenica on 23/09/2021.
//

#include "../../include/webserver.hpp"

void	display_server(std::vector<Server_conf> &servers)
{
	std::cout << "Check parsing : \n";

	for(std::vector<Server_conf>::iterator it = servers.begin(); it != servers.end(); ++it)
	{
		std::cout << "\n\nNew server conf\n";

		for (std::list<std::string>::iterator i_name = it->names.begin(); i_name != it->names.end(); ++i_name)
			std::cout << "server name : "<< *i_name << "\n";
		
		std::cout << "listen port : "<< it->listen_port << "\n";
		std::cout << "max upload size : "<< it->max_body_size << "\n";
		try {
			std::cout << "error page 404 : "<< it->error_page[404] << "\n";
		}
		catch(std::exception e) { std::cout << "no error page 404\n"; }

		for (std::map<std::string, Location>::iterator loc = it->locations.begin(); loc != it->locations.end(); ++loc)
		{
			std::cout << "path: " << loc->first << "\n";
			std::cout << "root: "<< loc->second.root << "\n";
			std::cout << "auto index : "<< ((loc->second.auto_index == true) ? "true" : "false") << "\n";
			std::cout << "index : "<< loc->second.index << "\n";
			std::cout << "upload path : "<< loc->second.upload_path << "\n";
			std::cout << "cgi path : "<< loc->second.cgi_path << "\n";
			std::cout << "cgi extension : "<< loc->second.cgi_extension << "\n";
			std::cout << "redirection : "<< loc->second.redirection << "\n";

			std::cout << "methods GET : "<< ((loc->second.methods[GET] == true) ? "true" : "false") << "\n";
			std::cout << "methods PUT : "<< ((loc->second.methods[PUT] == true) ? "true" : "false") << "\n";
			std::cout << "methods DELETE : "<< ((loc->second.methods[DELETE] == true) ? "true" : "false") << "\n";
			std::cout << "methods POST : "<< ((loc->second.methods[POST] == true) ? "true" : "false") << "\n";
		}
	}
}

