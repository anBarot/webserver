#include "../../include/webserver.hpp"

// check if the request line is valid. If not, an error code is set.
void	Request::check_line()
{
	if (request_line.version != "HTTP/1.1")
		code = HTTP_VERSION_NOT_SUPPORTED;
	else if (request_line.method == NOT_A_METHOD)
		code = BAD_REQUEST;

	if (code != DEFAULT)
		status = FINISH_PARSING;
}

/*
	check if a payload must be extracted and how (length or chunked).
	If not, the request status is set as finised.
*/
void	Request::check_payload()
{
	if (request_line.method != GET)
	{
		if (headers.count("transfer-encoding") &&
			headers["transfer-encoding"].find("chunked") != std::string::npos)
			payload.is_chunked = true;
		else if (headers.count("content-length"))
			payload.length = atoi(headers["content-length"].c_str());
		else
			status = FINISH_PARSING;
	}
	else
		status = FINISH_PARSING;
}

/*
	Check if a trailer is expected and header values are allowed.
	The expected header in trailer are put in the expected_trailers set.
*/
void	Request::check_trailer()
{
	std::string s[] = {"transfer-encoding", "content-length", "host", 
	"cache-control", "max-forwards", "te", "authorization", "set-cookie",
	"content-encoding", "content-type", "content-range", "trailer"};
	std::set<std::string> disallowed_trailer(s, s + 12);

	if (payload.is_chunked == true && headers.count("trailer"))
	{
		std::istringstream iss(headers["trailer"]);
		std::string word;
		
		while (iss >> word)
		{
			strlower(word);
			if (disallowed_trailer.find(word) == disallowed_trailer.end())
				expected_trailers.insert(word);
		}
	}
}
