#ifndef ERROR_HPP
# define ERROR_HPP

#include "cpp_libraries.hpp"

enum	e_error
{
	ARG_ERR,
	FILE_ERR,
	OTHER
};

struct err_cat
{
	private:
		e_error	err_no;
		std::string message(e_error error_nb) const
		{
			switch (error_nb)
			{
				case ARG_ERR :
					return "the accepted format is \"./webserver file.conf\"\n";
				case FILE_ERR :
					return "Can't open file\n";
				default :
					return "Uncategorized error\n";
			}
		}
	
	public:
		err_cat(e_error err) : err_no(err) {}
		~err_cat() {}
		int act() const
		{
			std::cout << "Webserver error : " << message(err_no);
			return (err_no);
		}
};

#endif
