NAME		=	webserv

CXX			=	c++

CXXFLAGS	=	-Wall -Wextra -Werror -std=c++98 -fsanitize=address -g

SRC_FILES	=	main.cpp					\
				parsing/Log.cpp				\
				parsing/Config.cpp			\
				parsing/Request.cpp			\
				parsing/conf_check.cpp		\
				server/setup_server.cpp		\
				server/handle_clients.cpp	\
				response_cgi/Response.cpp	\
				response_cgi/Cgi.cpp		\

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