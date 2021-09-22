#include "../../include/webserver.hpp"

// ws_trim erases the whitespaces found after the last word of the string
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
