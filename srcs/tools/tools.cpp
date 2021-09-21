#include "../../include/webserver.hpp"

// ws_trim erases the whitespaces found after the last word of the string
void ws_trim(std::string& s) {

	if (s.empty())
		return;

	std::string::iterator p;
	for (p = s.end(); p != s.begin() && iswspace(*--p););

	if (!iswspace(*p))
		p++;

	s.erase(p, s.end());
}
