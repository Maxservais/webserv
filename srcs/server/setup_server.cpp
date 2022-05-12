#include "../../webserv.hpp"

void	close_sockets(std::vector<int> sockets)
{
	for (std::vector<int>::iterator it = sockets.begin(); it != sockets.end(); ++it)
	{
		close(*(it));
	}
}

void	setup_server(std::vector<int> &sockets, Config &config, std::vector<struct sockaddr_in> &sockaddr)
{
	int	optval;
	int	len = config.get_servers().size();

	for (int i = 0; i < len; ++i)
	{
		/* Create sockets */
		sockets[i] = socket(AF_INET, SOCK_STREAM, 0);
		if (sockets[i] < 0)
		{
			close_sockets(sockets);
			throw SocketErr();
		}

		/* Set sockets to be nonblocking. All of the sockets for the incoming connections
		will also be nonblocking since they will inherit that state from the listening socket. */
		if (fcntl(sockets[i], F_SETFL, O_NONBLOCK) < 0)
		{
			close_sockets(sockets);
			throw SocketErr();
		}

		/* Allow re-use of the port */
		optval = 1;
		if (setsockopt(sockets[i], SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
		{
			close_sockets(sockets);
			throw SocketErr();
		}

		/* Bind each socket to the corresponding port */	
		if (bind(sockets[i], (struct sockaddr*)&sockaddr[i], sizeof(sockaddr[i])) < 0)
		{
			close_sockets(sockets);
			throw BindErr();
		}

		/* Wait for incoming connections */
		if (listen(sockets[i], SOMAXCONN) < 0)
		{
			close_sockets(sockets);
			throw ListenErr();
		}
	}
}

/* DOCUMENTATION:
- The socket() API returns a socket descriptor.
- The setsockopt() API allows the application to reuse the local address when the
server is restarted before the required wait time expires.
- After the socket descriptor is created, the bind() API gets a unique name for the socket.
- The listen() API call allows the server to accept incoming client connections.
- The fcntl() function changes the attributes of a socket descriptor.
It makes the socket descriptor non-blocking (meaning read or write operations
won't cause the thread to block). 
*/

