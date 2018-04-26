/*
**	Header - Basic server in C with MYSQL Databse integration
**
**
**
**
**
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


// ******************* Project related functions ******************** //

void professor(int sockfd, char *buf);
void aluno(int sockfd, char *buf);

// Operacoes dos alunos/professores
void list_codes(int sockfd);
void get_ementa(int sockfd);
void get_comment(int sockfd);
void get_full_info(int sockfd);
void get_all_info(int sockfd);

// Operacoes dos professores
void write_comment(int sockfd);

// ****************** Prints ***************************** //

void printa(char *msg);
void print_tela_inicial();
void print_ops_professor();
void print_ops_aluno();

// ************** [Server] - Basic functions ********************** //

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa);
void write_buffer(int sockfd, char *msg, int *msglen);
int read_buffer(int sockfd, char *msg, int *msglen);


int main(int argc, char *argv[])
{
	int sockfd;
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

	// -------------------------------------------------------------- //
	// ------------------------ MAIN CLIENT CODE -------------------- //
	// -------------------------------------------------------------- //


	int login = 1, size;

	while(login) {

		print_tela_inicial();

		scanf("%d", &login);

		switch(login) {

			case 1: //Professor

				//send professor code
				write_buffer(sockfd, "1", &size);
				professor(sockfd, buf);
				printf("Professor Logging out..\n");
			break;

			case 2: //Aluno

				//send aluno code
				write_buffer(sockfd, "2", &size);
				aluno(sockfd, buf);
				printf("Aluno Logging out..\n");
			break;

			case 0: //Exit
				write_buffer(sockfd, "0", &size);
			break;

			default:
				printf("OpÃ§Ã£o InvÃ¡lida.\n");

		}
	}

	// -------------------------------------------------------------- //
	// ------------------------ MAIN CLIENT CODE -------------------- //
	// -------------------------------------------------------------- //

	close(sockfd);
	return 0;
}

// ******************* Project related functions ******************** //

void professor(int sockfd, char *buf) {

	int num = 1;

	while(num){
		print_ops_professor();

		int choice, size;

		scanf("%d", &choice);

		char opcode[12];
		sprintf(opcode, "%d", choice);

		if(choice){

			if(choice < 7 && choice > 0) {
				//SEND OP CODE
				write_buffer(sockfd, opcode, &size);

				switch (choice)
				{
					case 1:
						list_codes(sockfd);
						break;
					case 2:
						get_ementa(sockfd);
						break;
					case 3:
						get_comment(sockfd);
						break;
					case 4:
						get_full_info(sockfd);
						break;
					case 5:
						get_all_info(sockfd);
						break;
					case 6:
						write_comment(sockfd);
						break;
				}

				//temp APAGAR aviso do server
				read_buffer(sockfd, buf, &size);
				printf("What server has to say to you: %s\n", buf);
				//temp APAGAR
			}
			else {
				printa("Invalid Op Code!.\n");
			}

		}
		else{
			num = choice;
			write_buffer(sockfd, opcode, &size);
		}
	}
}

void aluno(int sockfd, char *buf) {

	int num = 1;

	while(num){
		print_ops_aluno();

		int choice, size;

		scanf("%d", &choice);

		char opcode[12];
		sprintf(opcode, "%d", choice);

		if(choice){

			if(choice < 6 && choice > 0){
				//SEND OP CODE
				write_buffer(sockfd, opcode, &size);

				switch (choice)
				{
					case 1:
						list_codes(sockfd);
						break;
					case 2:
						get_ementa(sockfd);
						break;
					case 3:
						get_comment(sockfd);
						break;
					case 4:
						get_full_info(sockfd);
						break;
					case 5:
						get_all_info(sockfd);
						break;
				}
				//temp APAGAR aviso do server
				read_buffer(sockfd, buf, &size);
				printf("What server has to say to you: %s\n", buf);
				//temp APAGAR
			}
			else{
				printa("Invalid Op Code!.\n");
			}
		}
		else{
			num = choice;
			write_buffer(sockfd, opcode, &size);
		}
	}
}

// ************** [Server] - Basic functions ********************** //

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

// ***************** WRITE AND READ SOCKET ***************** //

void write_buffer(int sockfd, char *msg, int *msglen) {

	printf("---> Sending this msg: %s\n\n", msg);

	int total = 0;        // how many bytes we’ve sent
  int bytesleft = *msglen; // how many we have left to send
  int n;

	n = send(sockfd, msglen, sizeof(int), 0); // sending header with size of the msg!

	if(n == -1) {
		//return -1;
	}
	else {
		while(total < *msglen) {
				n = send(sockfd, msg+total, bytesleft, 0);
				if (n == -1) { break; }
				total += n;
				bytesleft -= n;
		}
		*msglen = total; // return number actually sent here
	}

	//return n == -1 ? -1 : 0; // return -1 on failure, 0 on success


	//int num = write(sockfd, msg, msglen);

	//if (num < 0) {

	//	exit(0);
	//}
}

int read_buffer(int sockfd, char *msg, int *msglen) {

	int total = 0;        			// how many bytes we’ve received
	int n;

  printf("---> Reading..\n");

	//int num = read(sockfd, buffer, bufferlen);
	n = recv(sockfd, msg, sizeof(int), 0);
	if(n == -1) {
		return -1;
	}
	else {
		*msglen =	atoi(msg);		// header size
		int bytesleft = *msglen;
		while(total < *msglen) {
				n = recv(sockfd, msg+total, bytesleft, 0);
				if (n == -1) { break; }
				total += n;
				bytesleft -= n;
		}
		*msglen = total; // return number actually sent here
	}

  printf("---> What was read: %s\n\n", msg);

	//if (num < 0) {
		//perror("ERROR: Reading from socket didnt go well..");
		//exit(0);
	//}
}


// *********************** Operacoes ALUNO/PROFESSOR *********************** //

// Listar todos os códigos de disciplinas com seus respectivos títulos;
void list_codes(int sockfd)
{
  printf("\n---------------------------------------\n");
  printf(" 1 -> Listar codigos das disciplinas\n");
  printf("---------------------------------------\n\n");

  // Adicionar read_result;

	printf("\n---------------------------------------\n\n");
}

// Dado o código de uma disciplina, retornar a ementa;
void get_ementa(int sockfd)
{
  char search_code[6];

  printf("\n---------------------------------------\n");
  printf(" 2 -> Buscar ementa\n");
  printf("---------------------------------------\n\n");

  printf("Digite o codigo da disciplina desejada:\n");
  scanf("%s", search_code);

	// Adicionar send_parameter;
	// Adicionar read_result;

	printf("\n---------------------------------------\n\n");
}

// Dado o código de uma disciplina, retornar o texto de comentário sobre a próxima aula.
void get_comment(int sockfd)
{
  char search_code[6];

  printf("\n---------------------------------------\n");
  printf(" 3 -> Buscar comentario sobre a proxima aula\n");
  printf("---------------------------------------\n\n");

  printf("Digite o codigo da disciplina desejada:\n");
  scanf("%s", search_code);

	// Adicionar send_parameter;
  // Adicionar read_result;

	printf("\n---------------------------------------\n\n");
}

// Dado o código de uma disciplina, retornar todas as informações desta disciplina;
void get_full_info(int sockfd)
{
  char search_code[6];

  printf("\n---------------------------------------\n");
  printf(" 4 -> Listar informacoes de uma disciplina\n");
  printf("---------------------------------------\n\n");

  printf("Digite o codigo da disciplina desejada:\n");
  scanf("%s", search_code);

	// Adicionar send_parameter;
	write_buffer(sockfd, search_code, strlen(search_code));

	// Adicionar read_result;

	printf("\n---------------------------------------\n\n");
}

// Listar todas as informações de todas as disciplinas
void get_all_info(int sockfd)
{
  printf("\n---------------------------------------\n");
  printf(" 5 -> Listar informacoes de todas as disciplinas\n");
  printf("---------------------------------------\n\n");

	// Adicionar read_result;

	printf("\n---------------------------------------\n\n");
}

// *********************** Operacoes do PROFESSOR *********************** //

// Escrever um texto de comentário sobre a próxima aula de uma disciplina (apenas usuário professor)
void write_comment(int sockfd)
{
  char search_code[6], comment[500];

  printf("\n---------------------------------------\n");
  printf(" 6 -> Escrever comentario sobre a proxima aula de uma disciplina\n");
  printf("---------------------------------------\n\n");

  printf("Digite o codigo da disciplina desejada:\n");
  scanf("%s", search_code);

	// Adicionar send_parameter;
	write_buffer(sockfd, search_code, strlen(search_code));

  printf("\nDigite o comentario que deseja inserir em %s:\n", search_code);
  fgets(comment, sizeof(comment), stdin);

	// Adicionar send_parameter;
	write_buffer(sockfd, comment, strlen(comment));

	printf("\nComentario adicionado!!\n");

  printf("\n---------------------------------------\n\n");
}

// ***************************** Prints ***************************** //

void printa(char *msg){
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
