/*
** client.c -- a stream socket client demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#define PORT "3490" // the port client will be connecting to

#define MAXDATASIZE 1500 // max number of bytes we can get at once

#define BUFFER_SIZE 1024

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
	int sockfd;
	// int numbytes;
	// char buf[MAXDATASIZE];
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];


	char buffer[BUFFER_SIZE];

	if (argc != 2) {
	    fprintf(stderr,"Usage: Client Hostname\n");
	    exit(1);
	}

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("Client: socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			perror("Client: connect");
			close(sockfd);
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "Client: Failed to connect\n");
		return 2;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
			s, sizeof s);
	printf("Client: connecting to %s\n", s);

	freeaddrinfo(servinfo); // all done with this structure

	//Loop de requests

	// char request[50];

	int choice = 1;

	while (choice) {
		printf("Enter with a number:\n");
		// printf("1. \n");
		// printf("2. \n");
		// printf("3. \n");
		// printf("4. \n");
		// printf("5. \n");
		// printf("6. \n");
		printf("7. \n");
		printf("0. Exit\n");
		scanf("%d", &choice);
		while ((getchar()) != '\n');
		switch (choice) {

			case 7:

				printf("Please say something to me:\n");

				bzero(buffer, BUFFER_SIZE);

				// printf("BUFFER: %s\n", buffer);

				fgets(buffer, BUFFER_SIZE-1, stdin);

				int num = write(sockfd, buffer, strlen(buffer));

				if (num < 0) {
					perror("ERROR: Writing to socket didnt go well..");
					exit(0);
				}

				bzero(buffer, BUFFER_SIZE);

				num = read(sockfd, buffer, BUFFER_SIZE-1);

				if (num < 0) {
					perror("ERROR: Reading from socket didnt go well..");
					exit(0);
				}

				printf("This is what you asked for.. : %s\n", buffer);

				printf("hello\n");

				break;

			case 0:
				break;
			default:
				printf("Invalid argument\n");
				break;
		}
	}

	// while(strcmp(request,"quit\n") != 0) {
	//
	// }




	// if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
	//     perror("recv");
	//     exit(1);
	// }

	// buf[numbytes] = '\0';
	//
	// printf("Client: received '%s'\n",buf);

	close(sockfd);

	return 0;
}
