//
// Created by lubenica on 20/09/2021.
//

#ifndef SATOSHIS_DATA_STRUCT_HPP
#define SATOSHIS_DATA_STRUCT_HPP

# define BACKLOG 			32
# define BUFFER_SIZE 		1000

# define S_BADREQ			"Bad Request"


enum e_methods {
	GET,
	POST,
	DELETE,
	PUT
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
	OK = 200,
	BAD_REQUEST = 400,
    UNAUTHORIZED = 401,  
	LENGTH_REQUIRED = 411,
	HTTP_VERSION_NOT_SUPPORTED = 505
};

 		// 		"100"  Continue
        //   | "101"  Switching Protocols
        //   | "201"  Created
        //   | "202"  Accepted
        //   | "203"  Non-Authoritative Information
        //   | "204"  No Content
        //   | "205"  Reset Content
        //   | "206"  Partial Content
        //   | "300"  Multiple Choices
        //   | "301"  Moved Permanently
        //   | "302"  Found
        //   | "303"  See Other
        //   | "304"  Not Modified
        //   | "305"  Use Proxy
        //   | "307"  Temporary Redirect

		//	Errors
      
        //   | "403"  Forbidden
        //   | "404"  Not Found
        //   | "405"  Method Not Allowed
        //   | "406"  Not Acceptable
        //   | "407"  Proxy Authentication Required
        //   | "408"  Request Time-out
        //   | "409"  Conflict
        //   | "410"  Gone
        //   | 
        //   | "412"  Precondition Failed
        //   | "413"  Request Entity Too Large
        //   | "414"  Request-URI Too Large
        //   | "415"  Unsupported Media Type
        //   | "416"  Requested range not satisfiable
        //   | "417"  Expectation Failed
        //   | "500"  Internal Server Error
        //   | "501"  Not Implemented
        //   | "502"  Bad Gateway
        //   | "503"  Service Unavailable
        //   | "504"  Gateway Time-out
       


#endif //SATOSHIS_DATA_STRUCT_HPP
