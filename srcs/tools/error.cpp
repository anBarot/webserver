#include "../../include/webserver.hpp"

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
		case PARSE_ERR :
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
