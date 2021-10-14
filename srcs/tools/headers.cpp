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
		--i;
	if (i == 0)
		return ("text/plain");
	ext = std::string(filename, i + 1, filename.size() - i);

	if (MIME_types.count(ext))
		return (MIME_types[ext]);
	return ("application/octet-stream");
}
