NAME			= webserv
SRCS_DIR 		= srcs
SRCS_FILES		= \
					tools.cpp \
					webserver.cpp \
					conf_file_parser.cpp  \
					headers.cpp \
					Connections.cpp \
					Client.cpp \
					Response.cpp \
					Request.cpp \
					\
					# srcs/debug_display.cpp


SRCS			= $(addprefix $(SRCS_DIR)/, $(SRCS_FILES))

OBJS			= $(SRCS:.cpp=.o)

DEP 			= $(OBJS:.o=.d)

CXX				= clang++

CXXFLAGS		= -Wall -Werror -Wextra -g -std=c++98 -MMD -I ./include $(OPTS)

ARGS			= ./conf/webserv_configuration.conf

OPTS			= -D LOGGER


$(NAME):		$(OBJS)
				@echo $(OBJS)
				$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)
-include $(DEP)

all:			$(NAME)

run:			all
				./$(NAME) $(ARGS)

leaks:			
				$(MAKE) OPTS="-D LOGGER -D CHECK_LEAKS=50" re
				valgrind --leak-check=full ./$(NAME) $(ARGS)

test_telnet:	
				./tests/test_telnet.sh > tests/results.txt

log:
				$(MAKE) OPTS="-D LOGGER" re
				./$(NAME) $(ARGS)

clean:
				rm -f $(OBJS)
				rm -f $(DEP)

fclean:			clean
				rm -f $(NAME)

re:				fclean all

.PHONY:			all clean fclean re run log
