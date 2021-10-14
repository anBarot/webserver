#include "../../include/webserver.hpp"

void implement_header_GET(Request &req, std::map<std::string, std::string> &headers, std::string &path)
{
	headers["Content-Type"] = get_MIME(path);
	headers["Transfer-Encoding"] = "chunked";
}

void create_directory_listing(Response &res, std::string &path)
{



}

void get_index_file(Request &req, Response &rep, std::string &path)
{
	struct stat st;

	if (stat(path.c_str(), &st))
		req.code = NOT_FOUND;
	else
		rep.file_name = path;
}

void response_method_get(Client &client, Request &req, Server_conf &sv, Location &loc)
{
	struct stat st;
	std::string path;

	req.code = OK;
	get_absolute_path(path, loc);
	if (stat(req.request_line.target.c_str(), &st))
		req.code = NOT_FOUND;
	else if (S_ISDIR(st.st_mode))
	{
		if (loc.auto_index == false)
			get_index_file(req, client.response, path.append("/").append(loc.index));
		else
			create_directory_listing(client.response, req.request_line.target);
	}
	else if (S_ISREG(st.st_mode))
	{
		client.response.headers["Last-Modified"] = get_date(st.st_mtime);
		client.response.headers["Content-Length"] = st.st_size;
		client.response.file_name = req.request_line.target;
	}
}
