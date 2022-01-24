/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abarot <abarot@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/21 15:22:41 by abarot            #+#    #+#             */
/*   Updated: 2022/01/24 19:23:10 by abarot           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
#define LOCATION_HPP

#include "libraries.hpp"
#include "data_struct.hpp"

class Location
{
	public:
		std::string								path;
		std::string								root;
		std::string								index;
		bool									auto_index;
		std::string								upload_path;
		bool									methods[5];
		std::string								cgi_path;
		std::string								cgi_extension;
		std::pair<e_response_code, std::string>	redirection;

		Location() {
			root = "";
			path = "";
			index = "";
			upload_path = "";
			methods[GET] = false;
			methods[POST] = false;
			methods[DELETE] = false;
			methods[NOT_A_METHOD] = false;
			auto_index = true;
			cgi_path = "";
			cgi_extension = "";
		}
		~Location() {}
};

#endif //LOCATION_HPP
