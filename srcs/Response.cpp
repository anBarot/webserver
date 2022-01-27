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

void free_arguments(char **arg)
{
	for (size_t i = 0; arg[i]; i++)
		free(arg[i]);
	free(arg);
}

char **create_env_array(std::vector<std::string> env_vec)
{
	char **res;
	std::vector<std::string>::iterator it = env_vec.begin(); 

	if (!(res = (char **)calloc(sizeof(char**), env_vec.size() + 1)))
		return NULL;
	for (size_t i = 0; i != env_vec.size(); i++)
	{
		res[i] = strdup(it->c_str());
		it++;
	}
	return res;
}

char **create_cgi_env(Request &req)
{
	std::vector<std::string> env_vec;
	std::string &req_path = req.request_line.target;

	std::string script_name = req_path.substr(req_path.find_last_of("/") + 1, req_path.size());

	env_vec.push_back("CONTENT_TYPE=" + req.headers["content-type"]);
	env_vec.push_back("CONTENT_LENGTH=" + req.headers["content-length"]);
	env_vec.push_back("PATH_INFO=" + req_path);
	env_vec.push_back("QUERY_STRING=" + get_query(req.payload.tmp_file_name));
	env_vec.push_back("REQUEST_METHOD=" + get_method_string(req.request_line.method));
	env_vec.push_back("SCRIPT_NAME=" + script_name);
	env_vec.push_back("SCRIPT_FILENAME=" + req_path);
	env_vec.push_back("SERVER_NAME=webserver");

	return create_env_array(env_vec);
}

void Response::create_cgi_file(Request &req, Location &loc)
{
	char **cgi_env;
	int status;
	std::string script_name;
	std::string exec_name;
	std::string &target = req.request_line.target;

	script_name = target.substr(target.find_last_of("/") + 1, target.size());
	exec_name = loc.cgi_path + "/" + script_name;
	cgi_env = create_cgi_env(req);
	status = exec_cgi(exec_name.c_str(), cgi_env);
	if (status == 1)
		code = NOT_FOUND;
	free_arguments(cgi_env);
}

int Response::exec_cgi(const char *exec_arg, char **cgi_env)
{
	pid_t pid;
	int cgi_file_fd;
	int status;
	char *const args[2] = {(char *)exec_arg, NULL};

	pid = fork();
	if (!pid)
	{
		close(STDIN_FILENO);
		cgi_file_fd = open("/tmp/tmp_cgi", O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
		if (cgi_file_fd == -1)
			exit(1);
		dup2(cgi_file_fd, STDOUT_FILENO);
		dup2(cgi_file_fd, STDERR_FILENO);
		if ((execve(args[0], args, cgi_env)) == -1)
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