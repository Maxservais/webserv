#include "webserv.hpp"

void	setup_server(int *sockfd, struct sockaddr_in *sockaddr)
{
	/* Create a socket */
	*sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (*sockfd < 0)
	{
		throw SocketErr();
	}

	/* Set socket to be nonblocking. All of the sockets for the incoming connections
	will also be nonblocking since they will inherit that state from the listening socket. */
	int rc = fcntl(*sockfd, F_SETFL, O_NONBLOCK);
	if (rc < 0)
	{
		close(*sockfd);
		throw SocketErr();
	}

	/* Fill in sockaddr struct */
	sockaddr->sin_family = PF_INET;
	sockaddr->sin_port = htons(SERVER_PORT);
	sockaddr->sin_addr.s_addr = INADDR_ANY;
	memset(sockaddr->sin_zero, '\0', sizeof sockaddr->sin_zero);

	/* Allow re-use of the port */
	int optval = 1;
	if (setsockopt(*sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
	{
		close(*sockfd);
		throw SocketErr();
	}


	/* Bind the socket to the port */
	if (bind(*sockfd, (struct sockaddr*)sockaddr, sizeof(*sockaddr)) < 0)
	{
		close(*sockfd);
		throw BindErr();
	}

	/* Wait for incoming connections */
	if (listen(*sockfd, SOMAXCONN) < 0)
	{
		close(*sockfd);
		throw ListenErr();
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

