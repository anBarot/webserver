//
// Created by lubenica on 20/09/2021.
//

#ifndef SATOSHIS_LOCATION_HPP
#define SATOSHIS_LOCATION_HPP

#include "cpp_libraries.hpp"
#include "data_struct.hpp"

class Location
{
	public:
		std::string		path;
		std::string		root;
		std::string		index;
		bool			auto_index;
		std::string		upload_path;
		bool			methods[4];
		std::string		cgi_path;
		std::string		cgi_extension;
		std::string		redirection;

		Location() {
			methods[GET] = false;
			methods[POST] = false;
			methods[DELETE] = false;
			methods[PUT] = false;
			methods[NOT_A_METHOD] = false;
			auto_index = false;
		}
		~Location() {}
};

#endif //SATOSHIS_LOCATION_HPP
