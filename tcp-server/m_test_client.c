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

#define MAXDATASIZE 100 // max number of bytes we can get at once

#define BUFFER_SIZE 100

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa);

// ***************** WRITE AND READ SOCKET *******************//

void write_buffer(int sockfd, char *msg);

void read_buffer(int sockfd, char *buffer);

// *****************

void login_ops(char *user_role, int sockfd, char *buffer);

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

		// printf("\n--------------------------------------- \n");
    // printf("  Bem vindo ao servidor de Disciplinas! \n");
    // printf("--------------------------------------- \n\n");
    //
    // printf("Operacoes disponiveis:\n");
    // printf(" 1 -> Login\n");
    // printf("-1 -> Encerrar sessao\n");
    //
    // printf("\nSelecione opcao desejada:\n");
    // scanf("%d", &menu_code);

		printf("\n--------------------------------------- \n");
		printf("Choose one of the operations below:\n");
		printf("--------------------------------------- \n\n");

		printf("1. Login\n");
		printf("0. Exit\n");
		printf("\n--------------------------------------- \n\n");
		scanf("%d", &choice);

		switch (choice) {

			case 1:

				printf("\n--> Login operation selected\n");
				//clearing buffer
				// scanf(" ");
				bzero(buffer, BUFFER_SIZE);
				// printf("oi\n");

				write_buffer(sockfd, "1", 1);


				char username[25], password[25];

				printf("Enter username: ");
				//scanf("%s", username);
				fgets(username, 25, stdin);
				// bzero(buffer, BUFFER_SIZE);
				write_buffer(sockfd, username, BUFFER_SIZE);

				// printf("%s\n", username);
				printf("Enter password: ");
				//scanf("%s", password);
				fgets(password, 25, stdin);
				// bzero(buffer, BUFFER_SIZE);
				write_buffer(sockfd, password, BUFFER_SIZE);

				char user_role[20];

				//read user_role
				read_buffer(sockfd, buffer, 20);

				strcpy(user_role, buffer);

				printf("user_role: %s\n", user_role);

				login_ops(user_role, sockfd, buffer);

				printf("-- Login ops ended --\n");

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




// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

// ***************** WRITE AND READ SOCKET *******************//

void write_buffer(int sockfd, char *msg) {

	printf("\n--> Sending this msg: %s\n\n", msg);

	if(send(sockfd, strlen(msg), sizeof(size_t), 0) == -1)
	{
		perror("send");
	}
	if(send(sockfd, msg, strlen(msg), 0) == -1)
	{
		perror("send");
	}

	//int num = write(sockfd, msg, strlen(msglen);

	if (num < 0) {
		perror("ERROR: Writing to socket didnt go well..");
		exit(0);
	}
}

void read_buffer(int sockfd, char *buffer){

  printf("\n--> Reading..\n");

	int num = read(sockfd, buffer, bufferlen);

  printf("--> What was read: %s\n\n", buffer);

	if (num < 0) {
		perror("ERROR: Reading from socket didnt go well..");
		exit(0);
	}
}


// ***********************************

void login_ops(char *user_role, int sockfd, char *buffer){

	int login = 1;

	char op_code[2];

	while(login){
		// Usuario eh Admin do BD.
		if(strcmp(user_role, "admin") == 0)
		{
			printf("\n--------------------------------------- \n");
			printf("Operacoes disponiveis:\n");
			printf("--------------------------------------- \n\n");
			printf(" 1 -> Listar usuarios\n");
			printf(" 2 -> Criar usuario\n");
			printf(" 3 -> Deletar usuario\n");
			printf("-1 -> Logout\n");
			printf("-2 -> Quit\n");
			printf("\n--------------------------------------- \n\n");

			printf("\nSelecione opcao desejada:\n");
			scanf("%s", op_code);

			write_buffer(sockfd, op_code, 2);

			if(strcmp(op_code ,"-1") == 0 || strcmp(op_code ,"-2") == 0) {
				login = 0;
			}

			//TODO

			// print respostas

		}
		// Usuario eh Aluno ou Professor.
		else
		{
			printf("\n--------------------------------------- \n");
			printf("Operacoes disponiveis:\n");
			printf("--------------------------------------- \n\n");
			printf(" 1 -> Listar codigos das disciplinas\n");
			printf(" 2 -> Buscar ementa\n");
			printf(" 3 -> Buscar comentario sobre a proxima aula\n");
			printf(" 4 -> Listar informacoes de uma disciplina\n");
			printf(" 5 -> Listar informacoes de todas as disciplinas\n");
			if(strcmp(user_role, "professor") == 0)
			{
				printf(" 6 -> Escrever comentario sobre a proxima aula de uma disciplina\n");
			}
			printf("-1 -> Logout\n");
			printf("-2 -> Quit\n");
			printf("\n--------------------------------------- \n\n");

			printf("\nSelecione opcao desejada:\n");
			scanf("%s", op_code);

			write_buffer(sockfd, op_code, 2);

			if(strcmp(op_code ,"-1") == 0 || strcmp(op_code ,"-2") == 0) {
				login = 0;
			}

			//TODO

			// print respostas


		}
	}
}
