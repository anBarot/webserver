//
// Created by lubenica on 20/09/2021.
//

#ifndef SATOSHIS_LOCATION_HPP
#define SATOSHIS_LOCATION_HPP

#include "cpp_libraries.hpp"
#include "error.hpp"



enum methods {
	GET,
	POST,
	DELETE,
	PUT
};

class Location
{
private:
	std::string						root;
	std::string						index;
	bool							auto_index;
	std::string						location;
	std::string						upload_path;
	bool							methods[4];
	std::string						cgi_path;

public:
	Location() {
		methods[GET] = false;
		methods[POST] = false;
		methods[DELETE] = false;
		methods[PUT] = false;
	}
	~Location() {}
};

#endif //SATOSHIS_LOCATION_HPP
