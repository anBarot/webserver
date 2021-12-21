#include <iostream>
#include <string>

int	check_http_version(std::string version)
{
	size_t pos = version.find_first_of("/");
	
	if (pos == std::string::npos)
		return (1);

	std::string value_part = version.substr(pos + 3, version.size());

    if (value_part.find_first_not_of("0123456789") != std::string::npos
        || version.substr(0, pos + 3) != "HTTP/1.")
        return 1;

    return 0;
}

int main(int ac, char **av)
{
    printf("check http value : %d\n", check_http_version(av[1]));
}
