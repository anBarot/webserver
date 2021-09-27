
NAME= webserv

SRCS=	srcs/webserver.cpp \
		srcs/socket_initialization/socket_initialization.cpp \
		srcs/vo.cpp \
		srcs/tools/tools.cpp \
		srcs/tools/error.cpp \
		srcs/tools/debug_display.cpp \
		srcs/parsing/parser.cpp

MAIN= webserver.cpp

all:
	clang++ -std=c++98  $(SRCS)  -o $(NAME)

clean:
	rm -rf $(NAME)

vo:
	clang++ -std=c++98  $(SRCS) srcs/vo.cpp -o vo_test