
NAME= webserv

SRCS=	srcs/webserver.cpp \
		srcs/socket_initialization/socket_initialization.cpp\
		srcs/tools/tools.cpp \
		srcs/parsing/parser.cpp\


all:
	clang++ -std=c++98  $(SRCS)  -o $(NAME)

clean:
	rm -rf $(NAME)