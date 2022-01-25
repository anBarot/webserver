#include "Response.hpp"

std::string get_query(std::string &file_name)
{
	std::ifstream payload_file(file_name.c_str());
	std::string str;
	std::string res;

	while (std::getline(payload_file, str))
		res = res + str;

	return res;
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

void free_arguments(char **arg)
{
	for (size_t i = 0; arg[i]; i++)
		free(arg[i]);
	free(arg);
}

char **create_env_array(std::map<std::string, std::string> env_map)
{
	char **res;
	std::string tmp;
	std::map<std::string, std::string>::iterator it = env_map.begin(); 

	if (!(res = (char **)calloc(sizeof(char**), env_map.size() + 1)))
		return NULL;
	for (size_t i = 0; i != env_map.size(); i++)
	{
		tmp = it->first + "=" + it->second;
		res[i] = strdup(tmp.c_str());
		it++;
	}

	return res;
}

char **create_cgi_env(Request &req)
{
	std::map<std::string, std::string> env_map;
	std::string &req_path = req.request_line.target;

	std::string script_name = req_path.substr(req_path.find_last_of("/") + 1, req_path.size());

	env_map["CONTENT_TYPE"] = req.headers["content-type"];
	env_map["CONTENT_LENGTH"] = req.headers["content-length"];
	env_map["PATH_INFO"] = req_path;
	env_map["QUERY_STRING"] = get_query(req.payload.tmp_file_name);
	env_map["REQUEST_METHOD"] = get_method_string(req.request_line.method);
	env_map["SCRIPT_NAME"] = script_name;
	env_map["SCRIPT_FILENAME"] = req_path;
	env_map["SERVER_NAME"] = "webserver";
	// env_map["HTTP_COOKIE"] = ;
	// env_map["HTTP_USER_AGENT"] = ;
	// env_map["REMOTE_ADDR"] = ;
	// env_map["REMOTE_HOST"] = ;
	
	return create_env_array(env_map);
}

char **create_exec_arg(std::string exec_path, std::string script)
{
	char **res;

	if (!(res = (char**)calloc(3, sizeof(char *))))
		return NULL;
	res[0] = strdup(exec_path.c_str());
	res[1] = strdup(script.c_str());

	return res;
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

int Response::exec_cgi(std::string exec_name, char **exec_arg, char **cgi_env)
{
	int fd[2];
	pid_t pid;
	int cgi_file_fd;
	int status;

	if (pipe(fd) == -1)
	{
		code = INTERNAL_SERVER_ERROR;
		return -1;
	}
	pid = fork();
	if (!pid)
	{
		close(fd[1]);
		dup2(fd[0], STDIN_FILENO);
		cgi_file_fd = open("/tmp/tmp_cgi", O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
		if (cgi_file_fd == -1)
			exit(1);
		dup2(cgi_file_fd, STDOUT_FILENO);
		dup2(cgi_file_fd, STDERR_FILENO);
		if ((execve(exec_name.c_str(), exec_arg, cgi_env)) == -1)
			exit(1);
		close(STDIN_FILENO);
		close(cgi_file_fd);
		close(fd[0]);
		exit(0);
	}
	else if (pid == -1)
	{
		code = INTERNAL_SERVER_ERROR;
		return -1;
	}
	close(fd[0]);
	close(fd[1]);
	waitpid(pid, &status, 0);

	return WEXITSTATUS(status);
}

void Response::create_cgi_file(Request &req, Location &loc)
{
	// std::cout << "Enterring CGI function\nrequest tmp file name : " << req.payload.tmp_file_name << "\n";
	char **cgi_env;
	char **exec_arg;
	int status;
	std::string script_name;
	std::string exec_name;
	std::string &target = req.request_line.target;

	script_name = target.substr(target.find_last_of("/") + 1, target.size());
	exec_name = loc.cgi_path + "/" + script_name;
	exec_arg = create_exec_arg(exec_name.c_str(), script_name);
	cgi_env = create_cgi_env(req);

	status = exec_cgi(exec_name, exec_arg, cgi_env);
	if (status == 1)
		code = NOT_FOUND;
	free_arguments(exec_arg);
	free_arguments(cgi_env);
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