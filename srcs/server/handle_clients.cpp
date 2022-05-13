#include "../../webserv.hpp"

int read_connection(int i, std::string &buff)
{
	char	buffer[BUFFER_SIZE + 1];
	static int x;

	/* Read from client' socket and add it to the buff string */
	memset(buffer, 0, BUFFER_SIZE);
	int ret = recv(i, buffer, BUFFER_SIZE, 0);
	x += ret;

	if (ret < 0)
		throw ConnectionErr();
	if (ret == 0)
		throw ConnectionClosedErr();
	buff += std::string(buffer, ret);

	/* Check whether "\r\n\r\n" was found */
	size_t res = buff.find("\r\n\r\n");
	if (res != std::string::npos)
	{
		if (buff.find("Content-Length: ") == std::string::npos)
			return 0;
		
		/* Find the content length */
		size_t len = std::atoi(buff.substr(buff.find("Content-Length: ") + 16, 10).c_str());
		
		/* If buff' size is bigger or equal to what the client told us he would send, then we are good */
		if (buff.size() >= len + res + strlen("\r\n\r\n"))
			return (0);
		/* Else, we exit and keep on receiving data from the socket */
		else
			return (1);
	}
	return (1);
}

void	send_data(int socket, const char *data, int len)
{
	int	bytes_sent;

	while(len > 0)
	{
		bytes_sent = send(socket, (char *)data, len, 0);
		if (bytes_sent == 0)
			break ;
		if (bytes_sent > 0)
		{
			data += bytes_sent;
			len -= bytes_sent;
		}
	}
}

void	disconnect_client(int client_fd, fd_set *current_sockets)
{
	close(client_fd);
	FD_CLR(client_fd, current_sockets);
}

void	handle_clients(int *sockets, Config &config, std::vector<struct sockaddr_in> &sockaddr)
{
	int				err;
	int				len = config.get_nb_port();
	int				max_socket_val = sockets[len - 1];
	fd_set			current_sockets;
	fd_set			ready_sockets;
	struct timeval	timeout;
	std::string		buff;

	/* Initiliaze current set */
	FD_ZERO(&current_sockets);

	/* Add each socket to the current set of file descriptors */
	for (int i = 0; i < len; ++i)
		FD_SET(sockets[i], &current_sockets);

	/* Loop, waiting for incoming connects or for incoming data on any of the connected sockets */
	while(true)
	{
		/* Timeout specifies how long we're willing to wait for a fd to become ready */
		timeout.tv_sec  = 3 * 60;
		timeout.tv_usec = 0;
		ready_sockets = current_sockets;
		err = select(max_socket_val + 1, &ready_sockets, NULL, NULL, &timeout);
		if (err < 0)
			throw SelectErr();
		else if (err == 0)
			throw TimeOutErr();
		for (int i = 0; i <= max_socket_val; i++)
		{
			if(FD_ISSET(i, &ready_sockets))
			{
				for (int j = 0; j < len; ++j)
				{
					/* If there is a new connection, accept it and add the new client socket
					to the current set of file descriptors */
					if (i == sockets[j])
					{
						socklen_t	addrlen = sizeof(sockaddr[j]);
						int	connection = accept(sockets[j], (struct sockaddr*)&sockaddr[j], &addrlen);
						if (connection < 0)
							throw AcceptErr();
						FD_SET(connection, &current_sockets);
						if (connection > max_socket_val)
							max_socket_val = connection;
						break ;
					}
					/* Else, handle the connection and then remove the socket from the set of FDs */
					else if (j == len - 1)
					{
						try
						{
							/* Read from client' socket */
							err = read_connection(i, buff);

							/* If the full request could be read, we parse it, build a response and send it */
							if (err == 0)
							{
								Request request(buff, config);
								buff.clear();
								Response resp(request);

								std::string response = resp.get_response();
								int len = response.size();
								const char *ret = response.c_str();
								send_data(i, ret, len); // send_data(i, response.c_str(), response.size());
								memset((void *)ret, 0, len);
								disconnect_client(i, &current_sockets);
							}
							else if (err == 1)
								break ;
						}
						catch (std::exception &e)
						{
							disconnect_client(i, &current_sockets);
							close_sockets(sockets, len);
							std::cerr << e.what() << std::endl;
							return ;
						}
					}
				}
			}
		}
	}
	close_sockets(sockets, len);
}

/* DOCUMENTATION:
- FD_CLR – Removes a socket from the set
- FD_ISSET – Helps in identifying if a socket belongs to a specified set
- FD_SET – Assigns a socket to a specified set
- FD_ZERO – Resets the set
- timeout specifies how long we're willing to wait for a fd to become ready
*/
