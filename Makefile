
NAME= webserv

SRCS=	srcs/socket_initialization/socket_initialization.cpp\
		srcs/tools/tools.cpp \
		srcs/parsing/parser.cpp\

MAIN= webserver.cpp

all:
	clang++ -std=c++98  $(SRCS)  -o $(NAME)

clean:
	rm -rf $(NAME)



vo:
	clang++ -std=c++98  $(SRCS) srcs/vo.cpp -o vo_test