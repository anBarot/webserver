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

		for (std::list<std::string>::iterator i_name = it->get_name().begin(); i_name != it->get_name().end(); ++i_name)
			std::cout << "server name : "<< *i_name << "\n";
		
		std::cout << "listen port : "<< it->get_listen_port() << "\n";
		std::cout << "max upload size : "<< it->get_max_body_size() << "\n";
		try {
			std::cout << "error page 404 : "<< it->get_error_page(404) << "\n";
		}
		catch(std::exception e) { std::cout << "no error page 404\n"; }

		for (std::list<Location>::iterator loc = it->get_location().begin(); loc != it->get_location().end(); ++loc)
		{
			std::cout << "\nFor location : "<< (*loc).get_path() << "\n";
			std::cout << "root: "<< (*loc).get_root() << "\n";
			std::cout << "auto index : "<< (((*loc).get_auto_index() == true) ? "true" : "false") << "\n";
			std::cout << "index : "<< (*loc).get_index() << "\n";
			std::cout << "upload path : "<< (*loc).get_upload_path() << "\n";
			std::cout << "cgi path : "<< (*loc).get_cgi_path() << "\n";
			std::cout << "cgi extension : "<< (*loc).get_cgi_extension() << "\n";
			std::cout << "redirection : "<< (*loc).get_redirection() << "\n";

			std::cout << "methods GET : "<< (((*loc).get_methods(GET) == true) ? "true" : "false") << "\n";
			std::cout << "methods PUT : "<< (((*loc).get_methods(PUT) == true) ? "true" : "false") << "\n";
			std::cout << "methods DELETE : "<< (((*loc).get_methods(DELETE) == true) ? "true" : "false") << "\n";
			std::cout << "methods POST : "<< (((*loc).get_methods(POST) == true) ? "true" : "false") << "\n";
		}
	}
}

