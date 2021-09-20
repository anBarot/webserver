#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#include "Server.hpp"
#include "Location.hpp"

//parser
int conf_parser(char *file_name);

//tools
void ws_trim(std::string& s);

#endif
