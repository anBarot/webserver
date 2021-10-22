#include "../../include/webserver.hpp"

void create_html_listing_file(std::string path, std::string listing_html)
{
	std::ifstream filein("html/listing.html");
	std::ofstream fileout("listing_temp.html");
	size_t pos_done;
	size_t pos_dtwo;
	std::string str;
 	
	while (std::getline(filein, str))
	{
		if ((pos_done = str.find("$1")) != std::string::npos)
			str.replace(pos_done, 2, path);
		if ((pos_dtwo = str.find("$2")) != std::string::npos)
			str.replace(pos_dtwo, 2, listing_html);
		fileout << str;
		fileout << "\n";
	}
	filein.close();
	fileout.close();
}

void Response::create_directory_listing(std::string &path)
{
	std::vector<std::string> files;
    struct dirent *entry;
	std::string listing_str;

	DIR *dir = opendir(path.c_str());
	while ((entry = readdir(dir)) != NULL)
		files.push_back(entry->d_name);
	closedir(dir);

	for (std::vector<std::string>::iterator it = files.begin(); it != files.end(); it++)
	{
		if (*it != "." && *it != "..")
			listing_str.append("<a href=\"").append(*it).append("\">").append(*it).append("</a>\n");
	}

	create_html_listing_file(path, listing_str);
	file_name = "listing_temp.html";
	headers["Content-Type"] = "text/html";
}

void Response::get_index_file(std::string &path)
{
	std::cout << "Index file : " << path << "\n";
	struct stat st;

	if (stat(path.c_str(), &st))
		code = NOT_FOUND;
	else
	{
		file_name = path;
		headers["Content-Type"] = get_MIME(path);
	}
}

void Response::method_get(Request &req, Location &loc, Server_conf &sv)
{
	std::cout << "Enterring method get\n";
	struct stat st;
	std::string path(req.request_line.target);

	req.code = OK;
	path.replace(0, loc.path.size(), loc.root);

	std::cout << "calling stat : " << path << "\n";
	if (stat(path.c_str(), &st))
	{
		std::cout << "dir not found\n";
		req.code = NOT_FOUND;
	}
	else if (S_ISDIR(st.st_mode))
	{
		if (loc.auto_index == false)
			get_index_file(path.append(loc.index));
		else
			create_directory_listing(path);
	}
	else if (S_ISREG(st.st_mode))
	{
		std::cout << "stat is reg\n";
		headers["Last-Modified"] = get_date(st.st_mtime);
		headers["Content-Type"] = get_MIME(path);
		file_name = req.request_line.target;
	}
	// headers["Transfer-Encoding"] = "chunked";
	std::cout << "quitting method get\n";
}

void Response::method_delete(Request &req, Location &loc)
{
	struct stat st;
	std::string path;

	req.code = OK;
	path.replace(0, loc.path.size(), loc.root);
	if (stat(path.c_str(), &st) || S_ISDIR(st.st_mode))
		req.code = NOT_FOUND;
	else
	{
		unlink(path.c_str());
		headers["Content-Type"] = "text/html";
		headers["Content-Length"] = "0";
	}
}

void Response::method_put(Request &req, Location &loc)
{
	struct stat st;
	std::string path;

	path.append(req.request_line.target).replace(0, loc.path.size(), loc.root);
	if (path[path.size() - 1] == '/')
	{
		code = BAD_REQUEST;
		return;
	}
	if (loc.upload_path.size())
		path = path.replace(0, path.find_last_of("/"), loc.upload_path);
	
	if (stat(path.c_str(), &st) == -1)
		code = CREATED;
	else
		code = NO_CONTENT;
	
	std::ifstream extr_file(req.payload.tmp_file_name.c_str());
	std::ofstream file;
	file.open(path.c_str(), std::ofstream::out | std::ofstream::trunc);
	file << extr_file.rdbuf();
	file.close();
	remove(req.payload.tmp_file_name.c_str());
	headers["Content-Location"] = path;
}

void Response::method_post(Request &req, Location &loc)
{



}
