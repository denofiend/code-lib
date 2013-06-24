/*
 * mx_socket.c
 *
 *  Created on: 2012-7-31
 *      Author: DenoFiend
 */
#include "tcp/mx_socket.h"
#include <sys/socket.h>

int tcp_client(char*serverHost, uint16_t serverPort, char*data, int length)
{
	struct sockaddr_in servaddr;
	int sockfd;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(serverPort);
	inet_pton(AF_INET, serverHost, &servaddr.sin_addr);

	if (0 == connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)))
	{

	}
	else
	{
		// connect error
		return -1;
	}

	return 0;
}

