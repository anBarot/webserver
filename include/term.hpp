/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   term.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adda-sil <adda-sil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/25 22:47:09 by adda-sil          #+#    #+#             */
/*   Updated: 2022/01/28 18:49:55 by adda-sil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TERM_HPP
# define TERM_HPP 1

# define RESET       "\033[0m"
# define BLACK       "\033[30m"             /* Black */
# define RED         "\033[31m"             /* Red */
# define GREEN       "\033[32m"             /* Green */
# define YELLOW      "\033[33m"             /* Yellow */
# define BLUE        "\033[34m"             /* Blue */
# define MAGENTA     "\033[35m"             /* Magenta */
# define CYAN        "\033[36m"             /* Cyan */
# define WHITE       "\033[37m"             /* White */
# define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
# define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
# define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
# define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
# define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
# define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
# define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
# define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

# ifdef LOGGER
#  define __WEBSERV_INTRO \
    std::cout << RED  		<< "██╗    ██╗███████╗██████╗ ███████╗███████╗██████╗ ██╗   ██╗" << std::endl; \
	std::cout << YELLOW  	<< "██║    ██║██╔════╝██╔══██╗██╔════╝██╔════╝██╔══██╗██║   ██║" << std::endl; \
	std::cout << GREEN 		<< "██║ █╗ ██║█████╗  ██████╔╝███████╗█████╗  ██████╔╝██║   ██║" << std::endl; \
	std::cout << BLUE 		<< "██║███╗██║██╔══╝  ██╔══██╗╚════██║██╔══╝  ██╔══██╗╚██╗ ██╔╝" << std::endl; \
	std::cout << MAGENTA    << "╚███╔███╔╝███████╗██████╔╝███████║███████╗██║  ██║ ╚████╔╝ " << std::endl; \
	std::cout << MAGENTA    << " ╚══╝╚══╝ ╚══════╝╚═════╝ ╚══════╝╚══════╝╚═╝  ╚═╝  ╚═══╝  " << std::endl; \
	std::cout << RESET 
# define __AWAIT_REQ \
    std::cout << YELLOW << "Awaiting request..." << RESET << std::endl
# else
#  define __WEBSERV_INTRO 
#  define __AWAIT_REQ 
# endif
#endif // !TERM_HPP