//
// Created by lubenica on 20/09/2021.
//

#ifndef SATOSHIS_LOCATION_HPP
#define SATOSHIS_LOCATION_HPP

#include "cpp_libraries.hpp"
#include "data_struct.hpp"

class Location
{
	private:
		std::string						root;
		std::string						index;
		bool							auto_index;
		std::string						path;
		std::string						upload_path;
		bool							methods[4];
		std::string						cgi_path;
		std::string						cgi_extension;
		std::string						redirection;

	public:
		Location() {
			methods[GET] = false;
			methods[POST] = false;
			methods[DELETE] = false;
			methods[PUT] = false;
			methods[NOT_A_METHOD] = false;
			auto_index = false;
		}
		~Location() {}

		// Getters
		std::string		get_root() 					{ return this->root; }
		std::string		get_index() 				{ return this->index; }
		bool			get_auto_index() 			{ return this->auto_index; }
		std::string		get_path() 					{ return this->path; }
		std::string		get_upload_path() 			{ return this->upload_path; }
		bool			get_methods(e_methods met)	{ return this->methods[met]; }
		std::string		get_cgi_path() 				{ return this->cgi_path; }
		std::string		get_cgi_extension() 		{ return this->cgi_extension; }
		std::string		get_redirection() 			{ return this->redirection; }

		// Setters
		void	set_root(std::string n_root) 				{ this->root = n_root; }
		void	set_index(std::string n_index) 				{ this->index = n_index; }
		void	set_auto_index(bool	autoi_val) 				{ this->auto_index = autoi_val; }
		void	set_path(std::string n_path) 				{ this->path = n_path; }
		void	set_upload_path(std::string up_path) 		{ this->upload_path = up_path; }
		void	set_methods(e_methods met, bool met_val)	{ this->methods[met] = met_val; }
		void	set_cgi_path(std::string n_cgi_path) 		{ this->cgi_path = n_cgi_path; }
		void	set_cgi_extension(std::string n_cgi_ext) 	{ this->cgi_extension = n_cgi_ext; }
		void	set_redirection(std::string n_redirection)	{ this->redirection = n_redirection; }
};

#endif //SATOSHIS_LOCATION_HPP
