#ifndef ERROR_HPP
# define ERROR_HPP

#include "cpp_libraries.hpp"
#include "data_struct.hpp"


struct err_cat
{
	private:
		e_error	err_no;
		std::string message(e_error error_nb) const
		{
			switch (error_nb)
			{
				case ARG_ERR :
					return "the accepted format is \"./webserver file.conf\"";
				case FILE_ERR :
					return "Can't open file";
				case SOCK_ERROR:
					return "Socket error";
				default :
					return "Uncategorized error";
			}
		}
	
	public:
		err_cat(e_error err) : err_no(err) {}
		~err_cat() {}
		int act() const
		{
			std::cout << "Webserver error : " << message(err_no) << std::endl;
			return (err_no);
		}
};

#endif
