#include "../../include/webserver.hpp"

void create_header_allow(std::map<std::string, std::string> &headers, Location &loc)
{
	std::stringstream sst;

	if (loc.get_methods(GET) == true)
		sst << "GET ";
	if (loc.get_methods(POST) == true)
		sst << "POST ";
	if (loc.get_methods(PUT) == true)
		sst << "PUT ";
	if (loc.get_methods(DELETE) == true)
		sst << "DELETE";
	sst << "\r\n";
	sst >> headers["allow"];
}

void implement_header_GET(Request &req, std::map<std::string, std::string> &headers, e_methods meth)
{
	if (req.headers.count("content-language")) 
		headers["content-language"] = ;
	if () 
		headers["Content-Length"] = ;
	if () 
		headers["Content-Type"] = ;
	headers["Date"] = get_date();
	if (req.code < 400) 
		headers["transfer-encoding"] = ;

	if (meth == GET)
	{
		if () 
			headers["Content-Location"] = ;
		if () 
			headers["Last-Modified"] = ;
		if () 
			headers["Location"] = ;
		if () 
			headers["Retry-After"] = ;
		if () 
			headers["Sever"] = ;
		if () 
			headers["WWW-Authenticate"] = ;
	}
}

void response_method_get(Client &client, Request &req, Server_conf &sv)
{
	struct stat st ;

	if (stat(req.request_line.target.c_str(), &st) != 0)
		req.code = NOT_FOUND;
	else if (S_ISDIR(st.st_mode) && sv.is_location(req.request_line.target) == true)
	{
		Location &loc = sv.get_location(req.request_line.target);

		if (loc.get_auto_index() == false)
			req.code = NOT_FOUND;
		else 
		{
			if (loc.get_methods(GET) == false)
			{
				req.code = METHOD_NOT_ALLOWED;
				create_header_allow(client.response.headers, loc);
			}
			else
				req.code = OK;
		}
	}
	else if (S_ISREG(st.st_mode))
	{

	}

	std::stringstream sst;
	sst << "HTTP/1.1 " << req.code << " " << reason_phrase[req.code] << "\r\n";
	sst >> client.response.line;
	if (req.code < 400)
		implement_response_header(req, client.response.headers, GET);
}
