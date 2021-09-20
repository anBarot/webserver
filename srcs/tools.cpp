#include "tools.hpp"

void ws_trim(std::string& s) {

	if (s.empty())
		return;

	std::string::iterator p;
	for (p = s.end(); p != s.begin() && iswspace(*--p););

	if (!iswspace(*p))
		p++;

	s.erase(p, s.end());
	s.erase(0, s.find_first_not_of(" \t\r\n"));
}
