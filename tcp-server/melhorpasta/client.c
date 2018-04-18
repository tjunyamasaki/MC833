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

#define PORT "8000" // the port client will be connecting to

#define MAXDATASIZE 100 // max number of bytes we can get at once

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}



// ***************** WRITE AND READ SOCKET *******************//
void write_buffer(int sockfd, char *msg, int msglen) {

	printf("\n--> Sending this msg: %s\n\n", msg);

	int num = write(sockfd, msg, msglen);

	if (num < 0) {
		perror("ERROR: Writing to socket didnt go well..");
		exit(0);
	}
}

void read_buffer(int sockfd, char *buffer, int bufferlen){

  printf("\n--> Reading..\n");

	int num = read(sockfd, buffer, bufferlen);

  printf("--> What was read: %s\n\n", buffer);

	if (num < 0) {
		perror("ERROR: Reading from socket didnt go well..");
		exit(0);
	}
}

/*************** Prints **********/

void print_1(char *msg);
void print_tela_inicial();
void print_ops_professor();
void print_ops_aluno();


/************************/
void professor(int sockfd, char *buf);

void aluno(int sockfd, char *buf);





int main(int argc, char *argv[])
{
	int sockfd, numbytes;
	char buf[MAXDATASIZE];
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];

	if (argc != 2) {
	    fprintf(stderr,"usage: client hostname\n");
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
			perror("client: socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			perror("client: connect");
			close(sockfd);
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		return 2;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
			s, sizeof s);
	printf("client: connecting to %s\n", s);

	freeaddrinfo(servinfo); // all done with this structure

	// if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
	//     perror("recv");
	//     exit(1);
	// }
	//
	// buf[numbytes] = '\0';
	//
	// printf("client: received '%s'\n",buf);

	// ------------------------MEU CODIGO ---------------------

	int login = 1;

	while(login) {

		print_tela_inicial();

		scanf("%d", &login);

		switch(login) {

			case 1: //Professor

				//send professor code
				write_buffer(sockfd, "1", 1);
				professor(sockfd, buf);
				printf("Professor Logging out..\n");
			break;

			case 2: //Aluno
				//send aluno code
				write_buffer(sockfd, "2", 1);
				aluno(sockfd, buf);
				printf("Aluno Logging out..\n");
			break;

			case 0: //Exit
			break;

			default:
				printf("Opção Inválida.\n");

		}
	}



	// ------------------------MEU CODIGO ---------------------


	close(sockfd);

	return 0;
}



void professor(int sockfd, char *buf) {

	int num = 1;

	while(num){
		print_ops_professor();

		int choice;

		scanf("%d", &choice);

		if(choice){
			//SEND OP CODE
			write_buffer(sockfd, "1", 1);

			//READ ANSWER
			read_buffer(sockfd, buf, 12);

			printf("What server has to say to you: %s\n", buf);

		}
		else{
			num = choice;
			write_buffer(sockfd, "0", 1);
		}
	}
}

void aluno(int sockfd, char *buf) {

	int num = 1;

	while(num){
		print_ops_aluno();

		int choice;

		scanf("%d", &choice);

		if(choice){
			//SEND OP CODE
			write_buffer(sockfd, "1", 1);

			//READ ANSWER
			read_buffer(sockfd, buf, 12);

			printf("What server has to say to you: %s\n", buf);
		}
		else{
			num = choice;
			write_buffer(sockfd, "0", 1);
		}
	}
}

void print_1(char *msg){
	printf("%s\n", msg);
}

void print_tela_inicial(){
	printf("\n--------------------------------------- \n");
	printf("Choose one of the operations below:\n");
	printf("--------------------------------------- \n\n");

	printf("1. Login Professor\n");
	printf("2. Login Aluno\n");
	printf("0. Exit\n");
	printf("\n--------------------------------------- \n\n");
}

void print_ops_professor(){

	printf("\n--------------------------------------- \n");
	printf("Operacoes disponiveis:\n");
	printf("--------------------------------------- \n\n");
	printf(" 0. Logout\n");
	printf(" 1. Listar codigos das disciplinas\n");
	printf(" 2. Buscar ementa\n");
	printf(" 3. Buscar comentario sobre a proxima aula\n");
	printf(" 4. Listar informacoes de uma disciplina\n");
	printf(" 5. Listar informacoes de todas as disciplinas\n");
	printf(" 6. Escrever comentario sobre a proxima aula de uma disciplina\n");
	printf("\n--------------------------------------- \n\n");

}

void print_ops_aluno(){

	printf("\n--------------------------------------- \n");
	printf("Operacoes disponiveis:\n");
	printf("--------------------------------------- \n\n");
	printf(" 0. Logout\n");
	printf(" 1. Listar codigos das disciplinas\n");
	printf(" 2. Buscar ementa\n");
	printf(" 3. Buscar comentario sobre a proxima aula\n");
	printf(" 4. Listar informacoes de uma disciplina\n");
	printf(" 5. Listar informacoes de todas as disciplinas\n");
	printf("\n--------------------------------------- \n\n");

}
