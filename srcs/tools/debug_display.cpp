//
// Created by lubenica on 23/09/2021.
//

#include "../../include/webserver.hpp"

void display_server(Server_conf &sv)
{
	std::cout << "Check server content :\n";

	for (std::list<std::string>::iterator i_name = sv.names.begin(); i_name != sv.names.end(); ++i_name)
		std::cout << "server name : "<< *i_name << "\n";

	std::cout << "listen port : "<< sv.listen_port << "\n";
	std::cout << "max upload size : "<< sv.max_body_size << "\n";
	try {
		std::cout << "error page 404 : "<< sv.error_page[404] << "\n";
	}
	catch(std::exception e) { std::cout << "no error page 404\n"; }

	for (std::map<std::string, Location>::iterator loc = sv.locations.begin(); loc != sv.locations.end(); ++loc)
		display_location(loc->second);
}

void display_location(Location &loc)
{
	std::cout << "Check location content :\n";

	std::cout << "path : " << loc.path << "\n";
	std::cout << "root : " << loc.root << "\n";
	std::cout << "auto index : "<< ((loc.auto_index == true) ? "true" : "false") << "\n";
	std::cout << "index : "<< loc.index << "\n";
	std::cout << "upload path : "<< loc.upload_path << "\n";
	std::cout << "cgi path : "<< loc.cgi_path << "\n";
	std::cout << "cgi extension : "<< loc.cgi_extension << "\n";
	std::cout << "redirection : "<< loc.redirection << "\n";

	std::cout << "methods GET : "<< ((loc.methods[GET] == true) ? "true" : "false") << "\n";
	std::cout << "methods PUT : "<< ((loc.methods[PUT] == true) ? "true" : "false") << "\n";
	std::cout << "methods DELETE : "<< ((loc.methods[DELETE] == true) ? "true" : "false") << "\n";
	std::cout << "methods POST : "<< ((loc.methods[POST] == true) ? "true" : "false") << "\n";
}

void	display_servers(std::vector<Server_conf> &servers)
{
	std::cout << "Check parsing : \n";

	for(std::vector<Server_conf>::iterator it = servers.begin(); it != servers.end(); ++it)
		display_server(*it);
}

void display_response(Response &rep)
{
	std::cout << "Check response content :\n";

	std::cout << "Response line : " << rep.line << std::endl;
	std::cout << "Response headers : \n";

	for (std::map<std::string, std::string>::iterator it = rep.headers.begin(); it != rep.headers.end(); it++)
		std::cout << it->first << " : " << it->second << std::endl;
	
	std::cout << "code : " << rep.code << std::endl;
}

void	display_request(Request &req)
{
	std::cout << "Check parsing request :\n";

	std::cout << "Request line : " << req.request_line.method << " " 
	<< req.request_line.target << " " << req.request_line.version << "\n";
	
	std::cout << "Headers : \n"; 
	for (std::map<std::string, std::string>::iterator it = req.headers.begin(); it != req.headers.end(); it++)
		std::cout << it->first << " : " << it->second << "\n";

	std::cout << "code : " << req.code << std::endl;
}


