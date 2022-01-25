NAME			= webserv
SRCS_DIR 		= srcs
SRCS_FILES		= \
					webserver.cpp \
					conf_file_parser.cpp  \
					headers.cpp \
					tools.cpp \
					Connections.cpp \
					Client.cpp \
					Response.cpp \
					Request.cpp
					# srcs/debug_display.cpp

DEP 			= $(OBJS:.o=.d)

SRCS			= $(addprefix $(SRCS_DIR)/, $(SRCS_FILES))

OBJS			= $(SRCS:.cpp=.o)

CXX				= clang++

CXXFLAGS		= -Wall -Werror -Wextra -g -std=c++98 -MMD

-include $(DEP)

%.o:			%.cpp
				$(CXX) $(CXXFLAGS) -c $< -o $@ -I./include

all:			$(NAME)

$(NAME):		$(OBJS)
				$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

clean:
				rm -f $(OBJS)

fclean:			clean
				rm -f $(NAME)

re:				fclean all

.PHONY:			all clean fclean re