//
// Created by lubenica on 20/09/2021.
//

#ifndef SATOSHIS_DATA_STRUCT_HPP
#define SATOSHIS_DATA_STRUCT_HPP

# define BACKLOG 			32
# define BUFFER_SIZE 		1000

enum e_methods {
	GET,
	POST,
	DELETE,
	PUT,
	NOT_A_METHOD
};

enum	e_error {
	ARG_ERR,
	FILE_ERR,
	CONFFILE_PARSE_ERR,
	SOCK_ERR,
	REQ_ERR,
	OTHER
};

enum	e_request_status {
	STARTING_PARSING,
	LINE_PARSED,
	HEADER_PARSED,
	PAYLOAD_PARSED,
	FINISH_PARSING,
	TREATED
};

enum e_response_code {
	DEFAULT,
	CONTINUE = 100,
	SWITCHING_PROTOCOLS = 101,
	OK = 200,
	CREATED = 201,
	ACCEPTED = 202,
	NON_AUTHORITATIVE_INFORMATION = 203,
	NO_CONTENT = 204,
	RESET_CONTENT = 205,
	PARTIAL_CONTENT = 206,
	MULTIPLE_CHOICES = 300,
	MOVED_PERMANENTLY = 301,
	FOUND = 302,
	SEE_OTHER = 303,
	NOT_MODIFIED = 304,
	USE_PROXY = 305,
	TEMPORARY_REDIRECT = 307,
	BAD_REQUEST = 400,
	UNAUTHORIZED = 401,
	PAYMENT_REQUIRED = 402,
	FORBIDDEN = 403,
	NOT_FOUND = 404,
	METHOD_NOT_ALLOWED = 405,
	NOT_ACCEPTABLE = 406,
	PROXY_AUTHENTICATION_REQUIRED = 407,
	REQUEST_TIMEOUT = 408,
	CONFLICT = 409,
	GONE = 410,
	LENGTH_REQUIRED = 411,
	PRECONDITION_FAILED = 412,
	PAYLOAD_TOO_LARGE = 413,
	URI_TOO_LONG = 414,
	UNSUPPORTED_MEDIA_TYPE = 415,
	RANGE_NOT_SATISFIABLE = 416,
	EXPECTATION_FAILED = 417,
	UPGRADE_REQUIRED = 426,
	INTERNAL_SERVER_ERROR = 500,
	NOT_IMPLEMENTED = 501,
	BAD_GATEWAY = 502,
	SERVICE_UNAVAILABLE = 503,
	GATEWAY_TIMEOUT = 504,
	HTTP_VERSION_NOT_SUPPORTED = 505
};

extern std::map<int, std::string> reason_phrase;
extern std::map<std::string, std::string> MIME_types;

#endif //SATOSHIS_DATA_STRUCT_HPP
