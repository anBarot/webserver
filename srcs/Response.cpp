#include "Response.hpp"

std::string get_query(std::string &file_name)
{
	std::ifstream payload_file(file_name.c_str());
	std::stringstream buf;

	buf << payload_file.rdbuf();
	return buf.str();
}

int	is_cgi_compatible(Request &req, Location &loc)
{
	std::string &target = req.request_line.target;
	std::string ext;
	size_t pos;

	if (loc.cgi_path == "" || req.request_line.method == DELETE || 
		((pos = target.find_last_of(".")) == std::string::npos))
		return 0;
	ext = target.substr(pos + 1, target.size());
	for (std::vector<std::string>::iterator it = loc.cgi_extension.begin();
	it != loc.cgi_extension.end(); it++)
	{
		if (ext == *it)
			return 1;
	}
	return 0;
}

void create_html_listing_file(std::string path, std::string listing_html)
{
	std::ofstream fileout("./html/listing_temp.html");
 	
	fileout << "<!DOCTYPE html><html><head><title>Index of " << path << 
		"</title></head><body><h1>Index of " << path << "</h1><hr><pre>" << 
		listing_html <<"</pre><hr></body></html>";

	fileout.close();
}

void Response::create_directory_listing(std::string path, std::string loc_root, std::string loc_path)
{
	std::vector<std::string> files;
    struct dirent *entry;
	DIR *dir;
	std::string listing_str;
	std::string absolute_path;
	std::string relative_path;

	relative_path = path.substr(loc_root.size());
	if (relative_path[relative_path.size() - 1] != '/')
		relative_path.append("/");

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
			listing_str.append("<a href=\"").append(relative_path).append(*it).append("\">").append(*it).append("</a>\n");
		}
	}
	
	create_html_listing_file(relative_path, listing_str);
	file_name = "./html/listing_temp.html";
	headers["Content-Type"] = "text/html";
}

void Response::method_get(Request &req, Location &loc)
{
	struct stat st;
	std::string path(req.request_line.target);
	std::string path_index;

	if (is_cgi == true)
	{
		file_name = path;
		headers["Content-Type"] = "text/html";
		headers["Connection"] = "keep-alive";
		return ;
	}
	if (path[path.size() - 1] == '/')
		path.erase(path.size() - 1, 1);
	path = loc.root + path;
	path_index = path;
	path_index.append("/").append(loc.index);
	code = OK;
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

void Response::method_post(Request &req, Location &loc, Server_conf &sv)
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

std::vector<std::string> create_cgi_env(Request &req)
{
	std::vector<std::string> env;
	std::string &req_path = req.request_line.target;
	std::string script_name = req_path.substr(req_path.find_last_of("/") + 1, req_path.size());

	env.push_back("CONTENT_TYPE=" + req.headers["content-type"]);
	env.push_back("CONTENT_LENGTH=" + req.headers["content-length"]);
	env.push_back("PATH_INFO=" + req_path);
	env.push_back("QUERY_STRING=" + get_query(req.payload.tmp_file_name));
	env.push_back("REQUEST_METHOD=" + get_method_string(req.request_line.method));
	env.push_back("SCRIPT_NAME=" + script_name);
	env.push_back("SCRIPT_FILENAME=" + req_path);
	env.push_back("SERVER_NAME=webserver");

	return env;
}

void Response::create_cgi_file(Request &req, Location &loc)
{
	std::string script_name;
	std::string exec_name;
	std::string &target = req.request_line.target;

	script_name = target.substr(target.find_last_of("/") + 1, target.size());
	exec_name = loc.cgi_path + "/" + script_name;
	if (exec_cgi(exec_name.c_str(), create_cgi_env(req)) == 1)
		code = NOT_FOUND;
}

int Response::exec_cgi(const char *exec_arg, std::vector<std::string> cgi_env)
{
	pid_t pid;
	int cgi_file_fd;
	int status;

	std::vector<const char *> char_env;
	for (std::vector<const char *>::size_type i = 0; i < 8; ++i)
		char_env.push_back(cgi_env[i].c_str());

	char *const args[2] = {(char*)exec_arg, NULL};
	char *const env[9] = { (char*)char_env[0], (char *)char_env[1],
						(char*)char_env[2], (char*)char_env[3],
						(char*)char_env[4], (char*)char_env[5],
						(char*)char_env[6], (char*)char_env[7], 0 };

	pid = fork();
	if (!pid)
	{
		close(STDIN_FILENO);
		cgi_file_fd = open("/tmp/tmp_cgi", O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
		if (cgi_file_fd == -1)
			exit(1);
		dup2(cgi_file_fd, STDOUT_FILENO);
		dup2(cgi_file_fd, STDERR_FILENO);
		if ((execve(args[0], args, env)) == -1)
			exit(1);
	}
	else if (pid == -1)
	{
		code = INTERNAL_SERVER_ERROR;
		return -1;
	}
	waitpid(pid, &status, 0);
	return WEXITSTATUS(status);
}


void Response::extract_cgi_file()
{
	std::ifstream in_file("/tmp/tmp_cgi");
	std::ofstream out_file;
	std::string str;
	size_t pos_dpoint;

	file_name = std::tmpnam(NULL);
	out_file.open(file_name.c_str());
	while (getline(in_file, str) && is_header_str(str))
	{
		pos_dpoint = str.find_first_of(":");
		headers[str.substr(0, pos_dpoint)] = str.substr(pos_dpoint + 1, str.size());
	}
	while (getline(in_file, str))
		out_file << str + "\n";
	out_file.close();
}