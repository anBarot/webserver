#include "../../include/webserver.hpp"

std::string get_date(time_t now)
{
	char str[33];
	size_t size;
	
	tm *gmtm = gmtime(&now);

	size = strftime(str, 32, "%a, %d %b %Y %T GMT", gmtm);
	str[size] = '\0';
	return (std::string(str));
}

std::string get_MIME(std::string filename)
{
	std::string ext;
	size_t i;

	i = filename.size() - 1;
	while (i > 0 && filename[i] != '.')
		i--;
	if (!i)
		return ("text/plain");
	
	ext = std::string(filename, i + 1, filename.size() - i);
	if (MIME_types.count(ext))
		return (MIME_types[ext]);
	return ("application/octet-stream");
}

std::string	get_file_size(std::string file_name)
{
	struct stat st;
	std::stringstream ss;
    std::string res;

	if (stat(file_name.c_str(), &st) || !S_ISREG(st.st_mode))
		return "";

	ss << st.st_size;
	ss >> res;
	return (res);
}

std::string get_allow(Location &loc)
{
	std::string res;

	if (loc.methods[GET] == true)
		res.append("GET ");
	if (loc.methods[POST] == true)
		res.append("POST ");
	if (loc.methods[PUT] == true)
		res.append("PUT ");
	if (loc.methods[DELETE] == true)
		res.append("DELETE");
	return (res);
}

int is_header_str(std::string str)
{
	size_t pos_dpoint;
	size_t pos_header_end;

	pos_dpoint = str.find(":");
	pos_header_end = str.find_last_of("\r\n");
	if (pos_header_end == str.size() - 1 && pos_dpoint != 0 &&
		pos_dpoint != std::string::npos)
		return 1;

	return 0;
}
