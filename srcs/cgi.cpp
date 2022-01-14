/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abarot <abarot@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/21 15:27:19 by abarot            #+#    #+#             */
/*   Updated: 2022/01/06 15:09:39 by abarot           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserver.hpp"

void free_arguments(char **arg)
{
	for (size_t i = 0; arg[i]; i++)
		free(arg[i]);
	free(arg);
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

std::string get_query(std::string &file_name)
{
	std::ifstream payload_file(file_name.c_str());
	std::string str;
	std::string res;

	while (std::getline(payload_file, str))
		res = res + str;

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

int	is_cgi_compatible(Request &req, Location &loc)
{
	std::string &target = req.request_line.target;
	std::string ext;
	size_t pos;

	if (loc.cgi_path == "" || req.request_line.method == DELETE || 
		((pos = target.find_last_of(".")) == std::string::npos))
		return 0;
	ext = target.substr(pos + 1, target.size());
	if (ext == loc.cgi_extension)
		return 1;
	return 0;
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
