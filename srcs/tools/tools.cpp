#include "../../include/webserver.hpp"

// Erase the whitespaces found after the last word of the string
void ws_trim(std::string& s) {

	if (s.find("#") != std::string::npos)
		s.erase(s.find("#"));

	if (s.empty())
		return;

	std::string::iterator p;
	for (p = s.end(); p != s.begin() && iswspace(*--p););

	if (!iswspace(*p))
		p++;

	s.erase(p, s.end());
}

// Write the specific strerror and return the erno 
int error_and_exit(e_error error_nb)
{
	switch (error_nb)
	{
		case ARG_ERR :
			std::cout << "the accepted format is \"./webserver file.conf\"\n";
			break;
		case FILE_ERR :
			std::cout << "Can't open file\n";
			break;
		case CONFFILE_PARSE_ERR :
			std::cout << "Error during parsing, check your .conf file\n";
			break;
		case SOCK_ERR :
			std::cout << "Socket error\n";
			break;
		default :
			std::cout << "Uncategorized error\n";
			break;
	}

	return (error_nb);
}

// Check if the string is a method 
int	is_method(std::string str)
{
	if (str == "GET" || str == "PUT" || str == "DELETE" || str == "POST")
		return 0;

	return 1;
}

// Transform in lowercase every char in a string
void	strlower(std::string &str)
{
	for (size_t i = 0; i < str.size(); i++)
		str[i] = std::tolower(str[i]);
}
