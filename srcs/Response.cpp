#include "Response.hpp"


Response::Response(Request &request, Location &location) :
			req(request),
			code(request.response_code),
			sv(request.sv),
			loc(location),
			is_cgi(false)
{}

void create_autoindex_file(std::string path, std::string listing_html)
{
	std::ofstream fileout("./tmp/listing_temp.html");
 	
	fileout << "<!DOCTYPE html><html><head><meta charset=\"utf-8\"><title>Index of " 
		<< path << 
		"</title></head><body><h1>Index of " << path << "</h1><hr><pre>" << 
		listing_html <<"</pre><hr></body></html>";

	fileout.close();
}

void	create_error_file(int code)
{
	std::ofstream fileout("./tmp/error_temp.html");

	fileout << "<!DOCTYPE html><html><head><meta charset=\"utf-8\"><title>" 
			<< code << " "
			<< reason_phrase[code] << "</title></head><body><center><h1>"
			<< code << " " << reason_phrase[code] << "</h1></center><hr> \
			<center>webserver (Ubuntu/Mac OS)</center></body></html>";
	
	fileout.close();
}

void Response::create_directory_listing(std::string path, std::string loc_root)
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
	
	create_autoindex_file(relative_path, listing_str);
	file_name = "./tmp/listing_temp.html";
	headers["Content-Type"] = "text/html";
}


void Response::create_cgi_file()
{
	std::string script_name;
	std::string exec_name;
	std::string &target = req.request_line.target;

	script_name = target.substr(target.find_last_of("/") + 1, target.size());
	exec_name = loc.cgi_path + "/" + script_name;
	if (exec_cgi(exec_name.c_str()) == 1)
		code = NOT_FOUND;
}

int Response::exec_cgi(const char *exec_arg)
{
	pid_t pid;
	int cgi_file_fd;
	int status;
	char *empty_args[] = { NULL };

	pid = fork();
	if (!pid)
	{
		req.set_environment();
		close(STDIN_FILENO);
		cgi_file_fd = open("/tmp/tmp_cgi", O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
		if (cgi_file_fd == -1)
			exit(1);
		dup2(cgi_file_fd, STDOUT_FILENO);
 		if ((execve(exec_arg, empty_args, environ)) == -1)
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

	file_name = random_filename();
	out_file.open(file_name.c_str());
	while (getline(in_file, str) && is_header_str(str))
	{
		pos_dpoint = str.find_first_of(":");
		headers[str.substr(0, pos_dpoint)] = str.substr(pos_dpoint + 1, str.size());
	}
	while (getline(in_file, str))
		out_file << str + "\n";
	in_file.close();
	out_file.close();
}

std::string Response::get_response()
{
	headers["Server"] = "webserver";
	headers["Date"] = get_date(time(NULL));
	if (loc.redirection.first >= 300)
	{
		code = loc.redirection.first;
		file_name = sv.error_page[code];
		headers["Location"] = loc.redirection.second;
		headers["Connection"] = "close";
	}
	if (code < 300 && loc.methods[req.request_line.method] == false) 
		code = METHOD_NOT_ALLOWED;
	if (code < 300)
	{
		if (req.is_cgi_compatible(loc))
		{
			is_cgi = true;
			create_cgi_file();
			extract_cgi_file();
		}
		else if (req.request_line.method == GET)
			method_get();
		else if (req.request_line.method == POST)
			method_post();
		else if (req.request_line.method == DELETE)
			method_delete();
	}
	if (code >= 400)
	{
		if (code == METHOD_NOT_ALLOWED)
			headers["Allow"] = get_allow(loc);
		std::ifstream file(sv.error_page[code].c_str());
		if (sv.error_page.count(code) && file.is_open())
			file_name = sv.error_page[code];
		else
		{
			file_name = "./tmp/error_temp.html";
			create_error_file(code);
		}
		file.close();
	}
	if (file_name != "")
		headers["Content-Length"] = get_file_size(file_name);
	create_response();
	return response;
}

void Response::create_response()
{
	std::ifstream file(file_name.c_str());
	std::stringstream buf;

	buf << "HTTP/1.1 " << (int)code << " " << reason_phrase[code] << "\r\n";
	for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); it++)
		buf << it->first << ": " << it->second << "\r\n";
	buf << "\r\n" << file.rdbuf() << "\r\n";
	response = buf.str();
	file.close();
}

// methods
void Response::method_get()
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
		else if (loc.auto_index == true)
			create_directory_listing(path, loc.root);
		else
			code = FORBIDDEN;
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

void Response::method_delete()
{
	struct stat st;
	std::string path;
	size_t pos;

	pos = req.request_line.target.find_last_of("/");
	path = loc.root + req.request_line.target.substr(pos, req.request_line.target.size());
	if (is_dir(path))
		code = CONFLICT;
	else if (stat(path.c_str(), &st))
		code = NOT_FOUND;
	else if (remove(path.c_str()))
		code = FORBIDDEN;
	else
	{
		code = NO_CONTENT;
		headers["Content-Type"] = get_MIME(path);
		headers["Content-Length"] = "0";
		headers["Connection"] = "keep-alive";
	}
}

void Response::method_post()
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