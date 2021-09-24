//
// Created by lubenica on 20/09/2021.
//

#ifndef SATOSHIS_DATA_STRUCT_HPP
#define SATOSHIS_DATA_STRUCT_HPP

enum e_methods {
	GET,
	POST,
	DELETE,
	PUT
};

enum	e_error
{
	ARG_ERR,
	FILE_ERR,
	PARSE_ERR,
	SOCK_ERR,
	OTHER
};

#endif //SATOSHIS_DATA_STRUCT_HPP
