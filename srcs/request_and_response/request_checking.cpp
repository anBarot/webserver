#include "../../include/webserver.hpp"

// check if the request line is valid. If not, an error code is set.
void	check_line(Request &req)
{
	if (req.request_line.version != "HTTP/1.1")
		req.code = HTTP_VERSION_NOT_SUPPORTED;
	else if (req.request_line.method == NOT_A_METHOD)
		req.code = BAD_REQUEST;

	if (req.code != DEFAULT)
		req.status = FINISH_PARSING;
}

/*
	check if a payload must be extracted and how (length or chunked).
	If not, the request status is set as finised.
*/
void	check_payload(Request &req)
{
	if (req.request_line.method != GET)
	{
		if (req.headers.count("transfer-encoding") &&
			req.headers["transfer-encoding"].find("chunked") != std::string::npos)
			req.payload.is_chunked = true;
		else if (req.headers.count("content-length"))
			req.payload.length = atoi(req.headers["content-length"].c_str());
		else
			req.status = FINISH_PARSING;
	}
	else
		req.status = FINISH_PARSING;
}

/*
	Check if a trailer is expected and header values are allowed.
	The expected header in trailer are put in the expected_trailers set.
*/
void	check_trailer(Request &req)
{
	std::set<std::string> disallowed_trailer = {"transfer-encoding", 
	"content-length", "host", "cache-control", "max-forwards", "te", 
	"authorization", "set-cookie", "content-encoding", "content-type"
	"content-range", "trailer"};

	if (req.payload.is_chunked == true && req.headers.count("trailer"))
	{
		std::istringstream iss(req.headers["trailer"]);
		std::string word;
		
		while (iss >> word)
		{
			strlower(word);
			if (disallowed_trailer.find(word) == disallowed_trailer.end())
				req.expected_trailers.insert(word);
		}
	}
}
