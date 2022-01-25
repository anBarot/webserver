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

DEP 			= $(OBJS:.o=.d)

SRCS			= $(addprefix $(SRCS_DIR)/, $(SRCS_FILES))

OBJS			= $(SRCS:.cpp=.o)

CXX				= clang++

CXXFLAGS		= -Wall -Werror -Wextra -g -std=c++98 -MMD -I ./include $(OPTS)

ARGS			= ./conf_files/example2.conf

OPTS			= 

-include $(DEP)

all:			$(NAME)


$(NAME):		$(OBJS)
				$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS) $()

run:			all
				./$(NAME) $(ARGS)

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