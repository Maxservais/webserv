#include "webserv.hpp"

void setup_server(int *sockets, Config *config, int *sockfd, int *sockfd1, struct sockaddr_in *sockaddr, struct sockaddr_in *sockaddr1)
{
	(void)sockfd;
	(void)sockfd1;

	/* Create sockets */
	int	len = config->get_servers().size();

	for (int i = 0; i < len; ++i)
	{
		sockets[i] = socket(AF_INET, SOCK_STREAM, 0);
		std::cout << "socket: " << sockets[i] << std::endl;
		if (sockets[i] < 0)
			throw SocketErr();
	}
	// *sockfd = socket(AF_INET, SOCK_STREAM, 0);
	// if (*sockfd < 0)
	// {
	// 	throw SocketErr();
	// }
	// *sockfd1 = socket(AF_INET, SOCK_STREAM, 0);
	// if (*sockfd1 < 0)
	// {
	// 	throw SocketErr();
	// }

	/* Set socket to be nonblocking. All of the sockets for the incoming connections
	will also be nonblocking since they will inherit that state from the listening socket. */
	for (int i = 0; i < len; ++i)
	{
		if (fcntl(sockets[i], F_SETFL, O_NONBLOCK) < 0)
		{
			close(sockets[i]); // CLOSE ALL SOCKETS, not just the one that failed to be changed to non-blocking
			throw SocketErr();
		}
	}

	// int rc = fcntl(*sockfd, F_SETFL, O_NONBLOCK);
	// if (rc < 0)
	// {
	// 	close(*sockfd);
	// 	throw SocketErr();
	// }

	// int rc1 = fcntl(*sockfd1, F_SETFL, O_NONBLOCK);
	// if (rc1 < 0)
	// {
	// 	close(*sockfd);
	// 	throw SocketErr();
	// }

	/* Fill in sockaddr struct */
	sockaddr->sin_family = PF_INET;
	sockaddr->sin_port = htons(SERVER_PORT);
	sockaddr->sin_addr.s_addr = INADDR_ANY;
	memset(sockaddr->sin_zero, '\0', sizeof sockaddr->sin_zero);
	sockaddr1->sin_family = PF_INET;
	sockaddr1->sin_port = htons(SERVER_PORT1);
	sockaddr1->sin_addr.s_addr = INADDR_ANY;
	memset(sockaddr1->sin_zero, '\0', sizeof sockaddr1->sin_zero);

	/* Allow re-use of the port */
	// int optval = 1;
	// for (int i = 0; i < len; ++i)
	// {
	// 	optval = 1;
	// 	if (setsockopt(sockets[i], SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
	// 	{
	// 		close(sockets[i]);
	// 		throw SocketErr();
	// 	}
	// }

	// int optval = 1;
	// if (setsockopt(*sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
	// {
	// 	close(*sockfd);
	// 	throw SocketErr();
	// }
	// int optval1 = 1;
	// if (setsockopt(*sockfd1, SOL_SOCKET, SO_REUSEADDR, &optval1, sizeof(optval1)) == -1)
	// {
	// 	close(*sockfd1);
	// 	throw SocketErr();
	// }

	/* Bind the socket to the port */
	// for (int i = 0; i < len; ++i)
	// {
	// 	if (bind(sockets[i], (struct sockaddr*)sockaddr, sizeof(*sockaddr)) < 0)
	// 	{
	// 		close(sockets[i]);
	// 		throw BindErr();
	// 	}
	// }

	if (bind(sockets[0], (struct sockaddr*)sockaddr, sizeof(*sockaddr)) < 0)
	{
		close(sockets[0]);
		throw BindErr();
	}
	if (bind(sockets[1], (struct sockaddr*)sockaddr1, sizeof(*sockaddr1)) < 0)
	{
		close(sockets[1]);
		throw BindErr();
	}

	/* Wait for incoming connections */
	for (int i = 0; i < len; ++i)
	{
		if (listen(sockets[i], SOMAXCONN) < 0)
		{
			close(sockets[i]);
			throw ListenErr();
		}
	}
	// if (listen(*sockfd, SOMAXCONN) < 0)
	// {
	// 	close(*sockfd);
	// 	throw ListenErr();
	// }
	// if (listen(*sockfd1, SOMAXCONN) < 0)
	// {
	// 	close(*sockfd1);
	// 	throw ListenErr();
	// }
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

