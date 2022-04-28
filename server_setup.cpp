#include "webserv.hpp"

int setup_server()
{
	int			sockfd;
	struct sockaddr_in	sockaddr;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		return(ft_error("Failed to create socket!"));
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.s_addr = INADDR_ANY;
	sockaddr.sin_port = htons(SERVER_PORT);

	if (bind(sockfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0)
	{
		close(sockfd);
		return(ft_error("Failed to bind!"));
	}

	if (listen(sockfd, 8000) < 0) // INT_MAX ?
	{
		close(sockfd);
		return(ft_error("Failed to listen on socket!"));
	}
	return (sockfd);
}

std::string read_parse_request(int i, Log log)
{
	char		buffer[1000000];

	memset(buffer, 0, 1000000);
	read(i, buffer, 1000000);
	std::cout << buffer << std::endl;
	
	Request request(buffer);
	log.add_one(request);

	Response response(request, "ressources", "index.html", "error404.html", SERVER_PORT);
	return (response.get_response());
}

int select_loop(Log log)
{
	int			sockfd = 0;
	sockaddr_in	sockaddr;
	socklen_t addrlen = sizeof(sockaddr);
	fd_set current_sockets;
	fd_set ready_sockets;

	sockfd = setup_server();
	if(sockfd == -1)
		return(ft_error("Failed to create a server!"));

	FD_ZERO(&current_sockets);
	FD_SET(sockfd, &current_sockets);
	
	while(1)
	{
		ready_sockets = current_sockets;
		if (select(FD_SETSIZE, &ready_sockets, NULL, NULL, NULL) < 0)
			return(ft_error("Select issue!"));
		for (int i = 0; i < FD_SETSIZE; i++)
		{
			if(FD_ISSET(i, &ready_sockets))
			{
				if (i == sockfd)
				{
					int	connection = accept(sockfd, (struct sockaddr*)&sockaddr, &addrlen);
					if (connection < 0)
						return(ft_error("Failed to grab connection!"));
					FD_SET(connection, &current_sockets);
				}
				else
				{
					std::string response = read_parse_request(i, log);
					send(i, response.c_str(), response.size(), 0);

					FD_CLR(i, &current_sockets);
					close(i);
				}
			}
		}
	}
	close(sockfd);
}
