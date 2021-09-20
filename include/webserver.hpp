#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#include "cpp_libraries.hpp"
#include "data_struct.hpp"

#include "Server.hpp"
#include "Location.hpp"

//parser
int conf_parser(char *file_name);

//tools
void ws_trim(std::string& s);
int error_and_exit(e_error error_nb);

#endif
