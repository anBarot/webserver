
NAME= webserv

SRCS = 	srcs/webserver.cpp \
		srcs/vo.cpp \
		srcs/conf_file_parsing/conf_file_parser.cpp  \
		srcs/socket_initialization/socket_initialization.cpp \
		srcs/request_and_response/request_checking.cpp \
		srcs/request_and_response/fill_response.cpp \
		srcs/request_and_response/request_treatment.cpp \
		srcs/request_and_response/methods.cpp \
		srcs/cgi/cgi.cpp \
		srcs/tools/debug_display.cpp \
		srcs/tools/headers.cpp \
		srcs/tools/tools.cpp

MAIN = webserver.cpp

all:
	clang++ -std=c++98 -g $(SRCS)  -o $(NAME)

clean:
	rm -rf $(NAME)

vo:
	clang++ -std=c++98  $(SRCS) srcs/vo.cpp -o vo_test