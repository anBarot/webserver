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
	return ("unrecognized MIME type");
}

std::string	get_file_size(std::string file_name)
{
	struct stat st;
	std::stringstream ss;
    std::string res;

	if (stat(file_name.c_str(), &st) || !S_ISREG(st.st_mode))
		return 0;

	ss << st.st_size;
	ss >> res;
	return (res);
}