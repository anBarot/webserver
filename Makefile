NAME	=	webserv

SRCS = 	srcs/webserver.cpp \
		srcs/conf_file_parser.cpp  \
		srcs/headers.cpp \
		srcs/tools.cpp \
		srcs/Connections.cpp \
		srcs/Client.cpp \
		srcs/Response.cpp \
		srcs/Request.cpp
		# srcs/debug_display.cpp

OBJS	=	$(SRCS:.cpp=.o)

CC		=	clang++

FLAGS	=	-Wall -Werror -Wextra -g -std=c++98	

%.o:		%.cpp
			$(CC) $(FLAGS) -c $< -o $@ -I./include

all:		$(NAME)

$(NAME):	$(OBJS)
			$(CC) $(FLAGS) -o $(NAME) $(OBJS)

clean:
			rm -f $(OBJS)

fclean:		clean
			rm -f $(NAME)

re:			fclean all