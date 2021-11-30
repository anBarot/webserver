NAME	=	webserv

SRCS = 	srcs/webserver.cpp \
		srcs/conf_file_parsing/conf_file_parser.cpp  \
		srcs/socket_initialization/socket_initialization.cpp \
		srcs/request_and_response/request_checking.cpp \
		srcs/request_and_response/fill_response.cpp \
		srcs/request_and_response/request_treatment.cpp \
		srcs/request_and_response/methods.cpp \
		srcs/cgi/cgi.cpp \
		srcs/tools/debug_display.cpp \
		srcs/tools/headers.cpp \
		srcs/tools/tools.cpp \
		srcs/connections/Connections.cpp

OBJS	=	$(SRCS:.cpp=.o)

CC		=	clang++

FLAGS	=	-Wall -Werror -Wextra -g -std=c++98	

%.o:		%.cpp
			$(CC) $(FLAGS) -c $< -o $@ -Iinclude

all:		$(NAME)

$(NAME):	$(OBJS)
			$(CC) $(FLAGS) -o $(NAME) $(OBJS)

clean:
			rm -f $(OBJS)

fclean:		clean
			rm -f $(NAME)

re:			fclean all