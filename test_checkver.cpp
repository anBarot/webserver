#include <iostream>
#include <string>

int	check_http_version(std::string version)
{
	int pos;
	
	if ((pos = version.find_first_of("/")) == std::string::npos)
		return (1);

	std::string value_part = version.substr(pos + 1, version.size());

    if (version.substr(0, pos + 1) != "HTTP/" || std::atof(value_part.c_str()) > 1.100001
        || std::atof(value_part.c_str()) <= 0.0000000)
        return 1;

    return 0;
}

int main(int ac, char **av)
{
    printf("check http value : %d\n", check_http_version(av[1]));
}
