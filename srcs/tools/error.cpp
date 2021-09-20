#include "../../include/webserver.hpp"

using namespace std;

int error_and_exit(e_error error_nb)
{
	switch (error_nb)
	{
		case ARG_ERR :
			cout << "the accepted format is \"./webserver file.conf\"\n";
			break;
		case FILE_ERR :
			cout << "Can't open file\n";
			break;
		case PARSE_ERR :
			cout << "Error during parsing, check your .conf file\n";
			break;
		case SOCK_ERR :
			cout << "Socket error\n";
			break;
		default :
			cout << "Uncategorized error\n";
			break;
	}

	return (error_nb);
}
