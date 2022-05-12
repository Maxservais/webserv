#include "../../webserv.hpp"

int read_connection(int socket, std::string &buff)
{
	char	buffer[BUFFER_SIZE + 1];

	/* Read from client' socket and add it to the buff string */
	memset(buffer, 0, BUFFER_SIZE);
	int ret = recv(socket, buffer, BUFFER_SIZE, 0);
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
			return (0);
	
		/* Find the content length */
		size_t len = std::atoi(buff.substr(buff.find("Content-Length: ") + strlen("Content-Length: "), 10).c_str());
		
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

void	handle_clients(std::vector<int> &sockets, Config &config, Log log, std::vector<struct sockaddr_in> &sockaddr) // Retirer log
{
	(void)log; // Retirer log
	int					err;
	int					max_socket_val = sockets[config.get_servers().size() - 1];
	fd_set				current_sockets;
	fd_set				read_sockets;
	struct timeval		timeout;
	std::string			buff;
	std::vector<int>	clients;

	/* Initiliaze current set */
	FD_ZERO(&current_sockets);

	/* Add each socket to the current set of file descriptors */
	for (std::vector<int>::iterator it = sockets.begin(); it != sockets.end(); ++it)
		FD_SET(*it, &current_sockets);

	/* Loop, waiting for incoming connects or for incoming data on any of the connected sockets */
	while(true)
	{
		/* Timeout specifies how long we're willing to wait for a fd to become ready */
		timeout.tv_sec  = 3 * 60;
		timeout.tv_usec = 0;

		/* Copy sockets and fill write_sockets */
		read_sockets = current_sockets;

		/* Select to read and write without blocking */
		err = select(max_socket_val + 1, &read_sockets, NULL, NULL, &timeout);
		if (err < 0)
			throw SelectErr();
		else if (err == 0)
			throw TimeOutErr();
		
		/* Handle the client connection and then remove the socket from the set of FDs */
		for (std::vector<int>::iterator it = clients.begin(); it != clients.end(); ++it)
		{
			if(FD_ISSET(*it, &read_sockets))
			{
				try
				{
					/* Read from client' socket */
					err = read_connection(*it, buff);

					/* If the full request could be read, we parse it, build a response and send it */
					if (err == 0)
					{
						Request request(buff, config);
						buff.clear();

						Response resp(request);
						std::string response = resp.get_response();
						int len = response.size();
						std::cout << len << std::endl;
						const char *ret = response.c_str();
						send_data(*it, ret, len); // send_data(i, response.c_str(), response.size());
						memset((void *)ret, 0, len);
						disconnect_client(*it, &current_sockets);
						clients.erase(it); // clients.erase(*it) ?
					}
					else if (err == -1)
					{
						FD_CLR(*it, &current_sockets);
						FD_CLR(*it, &read_sockets);
						clients.erase(it);
					}
					break ;
				}
				catch (std::exception &e)
				{
					disconnect_client(*it, &current_sockets);
					close_sockets(sockets);
					std::cerr << e.what() << std::endl;
					return ;
				}
			}
		}

		/* If there is a new connection, accept it and add the new client socket
		to the current set of file descriptors */
		int i = 0;
		for (std::vector<int>::iterator it = sockets.begin(); it != sockets.end(); ++it, ++i)
		{
			if(FD_ISSET(*it, &read_sockets))
			{
				socklen_t	addrlen = sizeof(sockaddr[i]);
				int	connection = accept(*it, (struct sockaddr*)&sockaddr[i], &addrlen);
				if (connection < 0)
					throw AcceptErr();
				FD_SET(connection, &current_sockets);
				clients.push_back(connection);
				if (connection > max_socket_val)
					max_socket_val = connection;
				break ;
			}
		}
	}
	close_sockets(sockets);
}

/* DOCUMENTATION:
- FD_CLR – Removes a socket from the set
- FD_ISSET – Helps in identifying if a socket belongs to a specified set
- FD_SET – Assigns a socket to a specified set
- FD_ZERO – Resets the set
- timeout specifies how long we're willing to wait for a fd to become ready
*/
