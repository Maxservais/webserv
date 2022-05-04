#include "../webserv.hpp"

std::string build_response(int i, Log log) // reference or pointer for log?
{
	/* Parse request */
	char	buffer[BUFFER_SIZE];
	memset(buffer, 0, BUFFER_SIZE);
	int ret = read(i, buffer, BUFFER_SIZE); // should we use receiv instead?
	std::cout << buffer << std::endl;
	if (ret == -1)
			throw ConnectionErr();
	Request request(buffer);

	/* Log what needs to be logged */
	(void)log;
	// log.add_one(request);

	/* Build response */
	Response response(request, "ressources", "index.html", "error404.html", SERVER_PORT);
	
	/* Return response */
	return (response.get_response());
}

void	send_data(int socket, const char *data, int len)
{
	int	bytes_sent;

	while(len > 0)
	{
		bytes_sent = send(socket, (char *)data, len, 0);
		if (bytes_sent == -1)
			throw SendErr();
		data += bytes_sent;
		len -= bytes_sent;
	}
}

void	disconnect_client(int client_fd, fd_set *current_sockets)
{
	close(client_fd);
	FD_CLR(client_fd, current_sockets);
}

void	handle_clients(Log log, int *sockfd, struct sockaddr_in *sockaddr)
{
	int			max_socket_val = *sockfd;
	socklen_t	addrlen = sizeof(*sockaddr);
	fd_set		current_sockets;
	fd_set		ready_sockets;
	// add writing sets here
	struct timeval	timeout;

	/* Initiliaze current set */
	FD_ZERO(&current_sockets);

	/* Add sockfd to the current set of file descriptors */
	FD_SET(*sockfd, &current_sockets);

	while(true)
	{
		timeout.tv_sec  = 1;
		timeout.tv_usec = 0;
		ready_sockets = current_sockets;
		if (select(max_socket_val + 1, &ready_sockets, NULL, NULL, &timeout) < 0)
			throw SelectErr();

		for (int i = 0; i <= max_socket_val; i++)
		{
			if(FD_ISSET(i, &ready_sockets))
			{
				/* If there is a new connection, accept it and add the new client socket
				to the current set of file descriptors */
				if (i == *sockfd)
				{
					int	connection = accept(*sockfd, (struct sockaddr*)sockaddr, &addrlen);
					if (connection < 0)
						throw AcceptErr();
					FD_SET(connection, &current_sockets);
					if (connection > max_socket_val)
						max_socket_val = connection;
				}
				/* Else, handle the connection and then remove the socket from the set of FDs */
				else
				{
					try
					{
						std::string response = build_response(i, log);
						int len = response.size();
						const char *ret = response.c_str();
						send_data(i, ret, len);
						memset((void *)ret, 0, len);
						disconnect_client(i, &current_sockets);
					}
					catch (std::exception &e)
					{
						disconnect_client(i, &current_sockets);
						close(*sockfd);
						std::cerr << e.what() << std::endl;
					}
				}
			}
		}
	}
	close(*sockfd);
}

/* DOCUMENTATION:
- FD_CLR – Removes a socket from the set
- FD_ISSET – Helps in identifying if a socket belongs to a specified set
- FD_SET – Assigns a socket to a specified set
- FD_ZERO – Resets the set
*/
