#include "../../include/webserver.hpp"

void create_html_listing_file(std::string path, std::string listing_html)
{
	std::ifstream filein("./html/listing.html");
	std::ofstream fileout("./html/listing_temp.html");
	size_t pos_done;
	size_t pos_dtwo;
	std::string str;
 	
	path.append("/");
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

void Response::create_directory_listing(std::string path, std::string loc_root, std::string loc_path)
{
	std::vector<std::string> files;
    struct dirent *entry;
	DIR *dir;
	std::string listing_str;
	std::string absolute_path;

	if ((dir = opendir(path.c_str())) == NULL)
	{
		code = FORBIDDEN;
		return ;
	}
	while ((entry = readdir(dir)) != NULL)
		files.push_back(entry->d_name);
	closedir(dir);

	sort(files.begin(), files.end());
	if (loc_path[loc_path.size() - 1] != '/')
		loc_path.append("/");
	for (std::vector<std::string>::iterator it = files.begin(); it != files.end(); it++)
	{
		if (*it != ".")
		{
			absolute_path = path + "/" + *it;
			if (is_dir(absolute_path))
				it->append("/");
			listing_str.append("<a href=\"").append(loc_path).append(*it).append("\">").append(*it).append("</a>\n");
		}
	}
	
	create_html_listing_file(path.erase(0, loc_root.size()), listing_str);
	file_name = "./html/listing_temp.html";
	headers["Content-Type"] = "text/html";
}

void Response::method_get(Request &req, Location &loc, Server_conf &sv)
{
	struct stat st;
	std::string path(req.request_line.target);
	std::string path_index;

	if (path[path.size() - 1] == '/')
		path.erase(path.size() - 1, 1);
	path = loc.root + path;
	path_index = path;
	path_index.append("/").append(loc.index);

	code = OK;
	std::cout << "Method get with path : " << path << "\n";
	if (stat(path.c_str(), &st))
		code = NOT_FOUND;
	else if (S_ISDIR(st.st_mode))
	{
		if (loc.index != "" && is_reg(path_index))
		{
			file_name = path_index;
			headers["Content-Type"] = get_MIME(path_index);
		}
		else
			create_directory_listing(path, loc.root, loc.path);
	}
	else if (S_ISREG(st.st_mode))
	{
		headers["Last-Modified"] = get_date(st.st_mtime);

		std::ifstream file(path.c_str());
		if (file.good() == false)
			code = FORBIDDEN;
		else
		{
			file_name = path;
			headers["Content-Type"] = get_MIME(path);
		}
		file.close();
	}
}

void Response::method_delete(Request &req, Location &loc)
{
	struct stat st;
	std::string path;

	path = loc.root + req.request_line.target;

	if (is_dir(path))
		code = CONFLICT;
	else if (stat(path.c_str(), &st))
		code = NOT_FOUND;
	else
	{
		if (remove(path.c_str()))
			code = FORBIDDEN;
		else
		{
			headers["Content-Type"] = get_MIME(path);
			headers["Content-Length"] = "0";
			headers["Connection"] = "keep-alive";
		}
	}
	code = NO_CONTENT;
}

void Response::method_put(Request &req, Location &loc, Server_conf &sv)
{
	struct stat st;
	std::string path;
	std::string location_path;
	
	if (req.payload.tmp_file_name == "")
	{
		code = BAD_REQUEST;
		return ;
	}

	std::stringstream sst;
	sst << "http://" << sv.names.front() << ":" << sv.listen_port << req.request_line.target;
	location_path.append(sst.str());
	
	path = loc.root + req.request_line.target;

	if (is_dir(path) || path[path.size() - 1] == '/')
	{
		code = CONFLICT;
		return;
	}
	if (loc.upload_path.size())
		path = path.replace(0, path.find_last_of("/"), loc.upload_path);

	if (stat(path.c_str(), &st) == -1)
		code = CREATED;
	else
		code = NO_CONTENT;

	if (rename(req.payload.tmp_file_name.c_str(), path.c_str()))
		code = UNAUTHORIZED;
	else
	{
		headers["Location"] = location_path;
		headers["Connection"] = "keep-alive";
	}
}
