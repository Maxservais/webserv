NAME		=	webserv

CXX			=	c++

CXXFLAGS	=	-Wall -Wextra -Werror -std=c++98 -fsanitize=address -g

SRC_FILES	=	main.cpp						\
				srcs/parsing/Config.cpp			\
				srcs/parsing/Request.cpp		\
				srcs/server/setup_server.cpp	\
				srcs/server/handle_clients.cpp	\
				srcs/response_cgi/Cgi.cpp		\
				srcs/response_cgi/Response.cpp	\
				srcs/parsing/conf_check.cpp		\

OBJ_FILES	=	$(SRC_FILES:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJ_FILES)
	$(CXX) $(CXXFLAGS) $(OBJ_FILES) -o $(NAME)

%.cpp: %.c
	$(CXX) -c $(CXXFLAGS) -o $@ $<

clean:
	rm -rf $(OBJ_FILES)

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re