#include "../../include/webserver.hpp"

std::string get_date()
{
	char str[33];
	size_t size;
	
	time_t now = time(NULL);
	tm *gmtm = gmtime(&now);

	size = strftime(str, 32, "%a, %d %b %Y %T GMT", gmtm);
	str[size] = '\0';
	return (std::string(str));
}
