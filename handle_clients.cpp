#include "webserv.hpp"

std::string read_parse_request(int i, Log log) // reference or pointer for log
{
	char		buffer[1000000];

	memset(buffer, 0, 1000000);
	read(i, buffer, 1000000);
	std::cout << buffer << std::endl;
	
	Request request(buffer);
	(void)log;
	// log.add_one(request);

	Response response(request, "ressources", "index.html", "error404.html", SERVER_PORT);
	// need to deal with error here!!! If there is an issue, we need to remove client from the list of sockets!!!!
	// int nBytes = recv(blabla)
	// if ((0 == nBytes) || (SOCKET_ERROR == nBytes))
	// {
	// 	if (0 != nBytes) //Some error occurred, 
	// 	//client didn't close the connection
	// 	{
	// 		printf("\nError occurred while 
	// 	receiving on the socket: %d.", 
	// 	GetSocketSpecificError
	// 	(pClientContext->GetSocket()));
	// 	}
	// remove client from socket list!!!
// }
	return (response.get_response());
}

int	send_data(int socket, const char *data, int len)
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
	return (0);
}

void	handle_clients(Log log, int *sockfd, struct sockaddr_in *sockaddr)
{
	int			max_socket_val = *sockfd;
	socklen_t	addrlen = sizeof(*sockaddr);
	fd_set		current_sockets;
	fd_set		ready_sockets;

	/* Initiliaze current set */
	FD_ZERO(&current_sockets);

	/* Add sockfd to the current set of file descriptors */
	FD_SET(*sockfd, &current_sockets);
	
	while(true)
	{
		ready_sockets = current_sockets;
		if (select(max_socket_val + 1, &ready_sockets, NULL, NULL, NULL) < 0)
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
					std::string response = read_parse_request(i, log);
					int len = response.size();
					const char * ret = response.c_str();
					try
					{
						send_data(i, ret, len);

					}
					catch (std::exception &e)
					{
						// close sockets?
						std::cerr << e.what() << std::endl;
					}

					FD_CLR(i, &current_sockets);
					close(i);
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
