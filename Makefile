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
					Request.cpp


SRCS			= $(addprefix $(SRCS_DIR)/, $(SRCS_FILES))

OBJS			= $(SRCS:.cpp=.o)

DEP 			= $(OBJS:.o=.d)

CXX				= clang++

CXXFLAGS		= -Wall -Werror -Wextra -g -std=c++98 -MMD -I ./include $(OPTS)

OPTS			= -D LOGGER


$(NAME):		$(OBJS)
				@echo $(OBJS)
				$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)
-include $(DEP)

all:			$(NAME)

run:			all
				./$(NAME)

clean:
				rm -f $(OBJS)
				rm -f $(DEP)

fclean:			clean
				rm -f $(NAME)

re:				fclean all

.PHONY:			all clean fclean re run log