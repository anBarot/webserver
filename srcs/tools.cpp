/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tools.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abarot <abarot@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/21 15:34:38 by abarot            #+#    #+#             */
/*   Updated: 2022/01/05 15:57:51 by abarot           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserver.hpp"

// Check if the signal is a break signal
int has_telnet_breaksignal(ssize_t last_read, char *buffer)
{
	if (last_read >= 5)
	{
		for (ssize_t i = 0; i + 5 <= last_read; i++)
		{
			if (buffer[i + 0] == (char)255 && buffer[i + 1] == (char)244 &&
				buffer[i + 2] == (char)255 && buffer[i + 3] == (char)253 && buffer[i + 4] == (char)6)
				return 1;
		}
	}
	return 0;
}

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

// Transform in lowercase every char in a string
void	strlower(std::string &str)
{
	for (size_t i = 0; i < str.size(); i++)
		str[i] = std::tolower(str[i]);
}

// Get the method enum from string
e_methods	get_method_enum(std::string word)
{
	if (word == "GET")
		return (GET);
	else if (word == "POST")
		return POST;
	else if (word == "PUT")
		return PUT;
	else if (word == "DELETE")
		return DELETE;
	else
		return NOT_A_METHOD;
}

// Get the method string from enum
std::string get_method_string(e_methods enm)
{
	if (enm == GET)
		return ("GET");
	else if (enm == POST)
		return "POST";
	else if (enm == PUT)
		return "PUT";
	else if (enm == DELETE)
		return "DELETE";
	else
		return "";
}

// Return 1 if the path is a directory, 0 else
int	is_dir(std::string path)
{
	struct stat st;

	if (stat(path.c_str(), &st))
		return 0;
	if (S_ISDIR(st.st_mode))
		return 1;
	return 0;
}

// Return 1 if the path is a file, 0 else
int is_reg(std::string path)
{
	std::cout << "Index file : " << path << "\n";
	struct stat st;

	if (stat(path.c_str(), &st))
		return 0;
	else
		return 1;
}

// Init reason_phrase map 
void	maps_init_reason_phrase(std::map<int, std::string> &reason_phrase)
{
	reason_phrase[100] = "Continue",
	reason_phrase[101] = "Switching Protocols";
	reason_phrase[200] = "OK";
	reason_phrase[201] = "Created";
	reason_phrase[202] = "Accepted";
	reason_phrase[203] = "Non-Authoritative Information";
	reason_phrase[204] = "No Content";
	reason_phrase[205] = "Reset Content";
	reason_phrase[206] = "Partial Content";
	reason_phrase[300] = "Multiple Choices";
	reason_phrase[301] = "Moved Permanently";
	reason_phrase[302] = "Found";
	reason_phrase[303] = "See Other";
	reason_phrase[304] = "Not Modified";
	reason_phrase[305] = "Use Proxy";
	reason_phrase[307] = "Temporary Redirect";
	reason_phrase[400] = "Bad Request";
	reason_phrase[401] = "Unauthorized";
	reason_phrase[402] = "Payment Required";
	reason_phrase[403] = "Forbidden";
	reason_phrase[404] = "Not Found";
	reason_phrase[405] = "Method Not Allowed";
	reason_phrase[406] = "Not Acceptable";
	reason_phrase[407] = "Proxy Authentication Required";
	reason_phrase[408] = "Request Timeout";
	reason_phrase[409] = "Conflict";
	reason_phrase[410] = "Gone";
	reason_phrase[411] = "Length Required";
	reason_phrase[412] = "Precondition Failed";
	reason_phrase[413] = "Payload Too Large";
	reason_phrase[414] = "URI Too Long";
	reason_phrase[415] = "Unsupported Media Type";
	reason_phrase[416] = "Range Not Satisfiable";
	reason_phrase[417] = "Expectation Failed";
	reason_phrase[426] = "Upgrade Required";
	reason_phrase[500] = "Internal Server Error";
	reason_phrase[501] = "Not Implemented";
	reason_phrase[502] = "Bad Gateway";
	reason_phrase[503] = "Service Unavailable";
	reason_phrase[504] = "Gateway Timeout";
	reason_phrase[505] = "HTTP Version Not Supported";
}

// Init MIME_types map 
void	maps_init_MIME_types(std::map<std::string, std::string> &MIME_types)
{
	MIME_types["aac"] = "audio/aac";
	MIME_types["abw"] = "application/x-abiword";
	MIME_types["arc"] = "application/octet-stream";
	MIME_types["avi"] = "video/x-msvideo";
	MIME_types["azw"] = "application/vnd.amazon.ebook";
	MIME_types["bin"] = "application/octet-stream";
	MIME_types["bmp"] = "image/bmp";
	MIME_types["bz"] = "application/x-bzip";
	MIME_types["bz2"] = "application/x-bzip2";
	MIME_types["csh"] = "application/x-csh";
	MIME_types["css"] = "text/css";
	MIME_types["csv"] = "text/csv";
	MIME_types["doc"] = "application/msword";
	MIME_types["docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
	MIME_types["eot"] = "application/vnd.ms-fontobject";
	MIME_types["epub"] = "application/epub+zip";
	MIME_types["gif"] = "image/gif";
	MIME_types["htm"] = "text/html";
	MIME_types["html"] = "text/html";
	MIME_types["ico"] = "image/x-icon";
	MIME_types["ics"] = "text/calendar";
	MIME_types["jar"] = "application/java-archive";
	MIME_types["jpeg"] = "image/jpeg";
	MIME_types["jpg"] = "image/jpeg";
	MIME_types["js"] = "application/javascript";
	MIME_types["json"] = "application/json";
	MIME_types["mid"] = "audio/midi";
	MIME_types["midi"] = "audio/midi";
	MIME_types["mpeg"] = "video/mpeg";
	MIME_types["mpkg"] = "application/vnd.apple.installer+xml";
	MIME_types["odp"] = "application/vnd.oasis.opendocument.presentation";
	MIME_types["ods"] = "application/vnd.oasis.opendocument.spreadsheet";
	MIME_types["odt"] = "application/vnd.oasis.opendocument.text";
	MIME_types["oga"] = "audio/ogg";
	MIME_types["ogv"] = "video/ogg";
	MIME_types["ogx"] = "application/ogg";
	MIME_types["otf"] = "font/otf";
	MIME_types["png"] = "image/png";
	MIME_types["pdf"] = "application/pdf";
	MIME_types["ppt"] = "application/vnd.ms-powerpoint";
	MIME_types["pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
	MIME_types["rar"] = "application/x-rar-compressed";
	MIME_types["rtf"] = "application/rtf";
	MIME_types["sh"] = "application/x-sh";
	MIME_types["svg"] = "image/svg+xml";
	MIME_types["swf"] = "application/x-shockwave-flash";
	MIME_types["tar"] = "application/x-tar";
	MIME_types["tif"] = "image/tiff";
	MIME_types["tiff"] = "image/tiff";
	MIME_types["ts"] = "application/typescript";
	MIME_types["ttf"] = "font/ttf";
	MIME_types["txt"] = "text/plain";
	MIME_types["vsd"] = "application/vnd.visio";
	MIME_types["wav"] = "audio/x-wav";
	MIME_types["weba"] = "audio/webm";
	MIME_types["webm"] = "video/webm";
	MIME_types["webp"] = "image/webp";
	MIME_types["woff"] = "font/woff";
	MIME_types["woff2"] = "font/woff2";
	MIME_types["xhtml"] = "application/xhtml+xml";
	MIME_types["xls"] = "application/vnd.ms-excel";
	MIME_types["xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
	MIME_types["xml"] = "application/xml";
	MIME_types["xul"] = "application/vnd.mozilla.xul+xml";
	MIME_types["zip"] = "application/zip";
	MIME_types["3gp"] = "audio/3gpp";
	MIME_types["3g2"] = "audio/3gpp2";
	MIME_types["7z"] = "application/x-7z-compressed";
}
