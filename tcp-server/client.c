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

// Handling time
#include <sys/time.h>
typedef struct timeval TIME;

#define PORT "8000" // Porta a qual o cliente se conecta
#define MAXDATASIZE 100 // Numero maximo de bytes que sao enviados em um pacote

// ************** [Client/Server] - Basic functions ************** //

// Get sockaddr, IPv4 or IPv6
void *get_in_addr(struct sockaddr *sa);

// Funcao para lidar com entrada
void get_input(char *input, size_t maxlen);

// Funcoes para comunicacao
void write_buffer(int sockfd, char *msg);
void read_buffer(int sockfd, char *msg);

// ******************* Project related functions ******************** //

// Interface de login
void login(int sockfd);

// Interfaces do aluno e professor
void professor(int sockfd);
void aluno(int sockfd);

// Operacoes dos alunos/professores
void list_codes(int sockfd);
void get_ementa(int sockfd);
void get_comment(int sockfd);
void get_full_info(int sockfd);
void get_all_info(int sockfd);

// Operacoes dos professores
void write_comment(int sockfd);

// ****************** Prints ****************** //

void print_tela_inicial();
void print_ops_professor();
void print_ops_aluno();

// ****************** Time evaluation for communication ****************** //
int timeval_subtract(TIME *result, TIME *x, TIME *y);
void communication_time_eval(int sockfd);
void function_time_eval(void (*operation)(int), int sockfd, int opcode);

// ****************** MAIN CODE ****************** //

int main(int argc, char *argv[])
{
	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char ip[INET6_ADDRSTRLEN];

	if (argc != 2)
	{
	    fprintf(stderr,"Usage: ./client hostname\n");
	    exit(1);
	}

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0)
	{
		fprintf(stderr, "Getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// Loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next)
	{
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
		{
			perror("Client: socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
		{
			perror("Client: connect");
			close(sockfd);
			continue;
		}
		break;
	}

	if (p == NULL)
	{
		fprintf(stderr, "Client: failed to connect\n");
		return 2;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), ip, sizeof ip);
	printf("\n-------------------------------------------------------\n");
	printf("Client: connecting to %s\n", ip);
	printf("-------------------------------------------------------\n");

	freeaddrinfo(servinfo); // All done with this structure

	login(sockfd);  // Inicia 'aplicacao'

	close(sockfd);
	return 0;
}

// ********************************** FUNCOES ********************************** //

// ***************** [Server/Client] - Funcoes Basicas ***************** //

// Get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

// Previne inputs invalidos
void get_input(char *input, size_t maxlen)
{
	fgets(input, maxlen, stdin);
	input[strcspn(input, "\r\n")] = 0;
}

// *********** WRITE AND READ SOCKET *********** //

void write_buffer(int sockfd, char *msg)
{
	int bytesleft, numbytes;
	char header[4], *auxmsg;

	bytesleft =  strlen(msg);
	sprintf(header, "%d", bytesleft);

	numbytes = send(sockfd, header, 4, 0);
	if (numbytes < 0)
	{
		perror("\nERROR: Writing to socket didnt go well...\n");
		exit(0);
	}

	auxmsg = msg;
	do {
		numbytes = send(sockfd, auxmsg, bytesleft, 0);

		if (numbytes < 0)
		{
			perror("\nERROR: Writing to socket didnt go well...\n");
			exit(0);
		}

		auxmsg += numbytes;  // Atualiza parte da mensagem que deve enviar
		bytesleft -= numbytes;

	} while(bytesleft > 0);
}

void read_buffer(int sockfd, char *msg)
{
	char header[4], *workbuffer, *auxpointer;
	int numbytes, bytesleft, bytesrcv = 0;

	numbytes = recv(sockfd, header, 4, 0);  // Recebe header

	if (numbytes < 0)
	{
		perror("\nERROR: Reading from socket didnt go well...\n");
		exit(0);
	}
	else if (numbytes == 0)
	{
		perror("\nERROR: Connection closed by Server.\n");
		exit(0);
	}

	bytesleft = atoi(header);
	workbuffer = malloc(2*bytesleft * sizeof(char)); // Buffer para tratar msg recebida

	auxpointer = workbuffer;
	do {
		numbytes = recv(sockfd, auxpointer, bytesleft, 0);

		if (numbytes < 0)
		{
			perror("\nERROR: Reading from socket didnt go well...\n");
			exit(0);
		}
		else if (numbytes == 0)
		{
			perror("\nERROR: Connection closed by Server.\n");
			exit(0);
		}

		bytesrcv += numbytes;  // Bytes recebidos ate o momento
		auxpointer += numbytes;  // Atualiza parte do buffer onde inserir msg recebida
		bytesleft -= numbytes;  // Bytes que faltam chegar

	} while(bytesleft > 0);

	workbuffer[bytesrcv] = '\0';
	strncpy(msg, workbuffer, bytesrcv+1);  // Copia apenas msg que deveria ser recebida

	free(workbuffer);  // Descarta possiveis lixos
}

// ******************* Funcoes relacionadas ao projeto ******************** //

// *********** Interfaces *********** //

// Realiza login no servidor.
void login(int sockfd)
{
	int login;
	char input[10];

	printf("\n********************************************************\n");
	printf("\t BEM VINDO AO BANCO DE DISCIPLINAS!!\n");
	printf("********************************************************\n");

	do {
		print_tela_inicial();
		printf("Selecione uma opcao:\n");

		get_input(input, sizeof(input));
		login = atoi(input);
		write_buffer(sockfd, input);  // Envia codigo do login

		switch(login)
		{
			case 1: // Professor
				professor(sockfd);
				break;
			case 2: // Aluno
				aluno(sockfd);
				break;
			case 0: // Exit
				printf("\nFechando conexao.\n");
				break;
			default:
				printf("\nOperacao Inval¡da.\n");
		}
	} while(login);
}

void professor(int sockfd)
{
	int choice;
	char opcode[10];

	printf("\n-------------------------------------------------------\n");
	printf("\n\t\t*** Bem Vindo Professor! ***\n");

	do {
		print_ops_professor();

		printf("Selecione uma operacao:\n");

		get_input(opcode, sizeof(opcode));
		choice = atoi(opcode);

		// Envia OP Code
		write_buffer(sockfd, opcode);

		switch (choice)
		{
			case 1:
				//list_codes(sockfd); -> Old call. Without getting time.
				function_time_eval(list_codes, sockfd, choice);
				break;
			case 2:
				//get_ementa(sockfd);
				function_time_eval(get_ementa, sockfd, choice);
				break;
			case 3:
				//get_comment(sockfd);
				function_time_eval(get_comment, sockfd, choice);
				break;
			case 4:
				//get_full_info(sockfd);
				function_time_eval(get_full_info, sockfd, choice);
				break;
			case 5:
				//get_all_info(sockfd);
				function_time_eval(get_all_info, sockfd, choice);
				break;
			case 6:
				//write_comment(sockfd);
				function_time_eval(write_comment, sockfd, choice);
				break;
			case 7:
				communication_time_eval(sockfd); // Funcao 'escondida'. Apenas para time test
				break;
			case 0:
					printf("\nProfessor logging out...\n");
					break;
			default:
				printf("\nInvalid Op Code!\n");
			}
	} while(choice);
}

void aluno(int sockfd)
{
	int choice;
	char opcode[10];

	printf("\n-------------------------------------------------------\n");
	printf("\n\t\t*** Bem Vindo Aluno! ***\n");

	do {
		print_ops_aluno();

		printf("Selecione uma operacao:\n");
		get_input(opcode, sizeof(opcode));
		choice = atoi(opcode);
		write_buffer(sockfd, opcode);  // Envia OP Code

		switch (choice)
		{
			case 1:
				//list_codes(sockfd);
				function_time_eval(list_codes, sockfd, choice);
				break;
			case 2:
				//get_ementa(sockfd);
				function_time_eval(get_ementa, sockfd, choice);
				break;
			case 3:
				//get_comment(sockfd);
				function_time_eval(get_comment, sockfd, choice);
				break;
			case 4:
				//get_full_info(sockfd);
				function_time_eval(get_full_info, sockfd, choice);
				break;
			case 5:
				//get_all_info(sockfd);
				function_time_eval(get_all_info, sockfd, choice);
				break;
			case 0:
				printf("\nAluno logging out...\n");
				break;
			default:
				printf("\nInvalid Op Code.\n");
		}
	} while(choice);
}

// *********** Operacoes de ALUNO e PROFESSOR *********** //

// Listar todos os códigos de disciplinas com seus respectivos títulos;
void list_codes(int sockfd)
{
	char result[2500];

  printf("\n-------------------------------------------------------\n");
  printf(" 1 -> Listar codigos das disciplinas\n");
  printf("-------------------------------------------------------\n\n");

	read_buffer(sockfd, result);
	printf("%s", result);
	printf("********************************************************\n");
}

// Dado o código de uma disciplina, retornar a ementa;
void get_ementa(int sockfd)
{
	char result[2500];
  char search_code[10];

  printf("\n-------------------------------------------------------\n");
  printf(" 2 -> Buscar ementa\n");
  printf("-------------------------------------------------------\n\n");

  printf("Digite o codigo da disciplina desejada:\n");
	get_input(search_code, sizeof(search_code));
	write_buffer(sockfd, search_code);

	printf("\n");
	read_buffer(sockfd, result);
	printf("%s", result);
	printf("********************************************************\n");
}

// Dado o código de uma disciplina, retornar o texto de comentário sobre a próxima aula.
void get_comment(int sockfd)
{
	char result[2500];
  char search_code[10];

  printf("\n-------------------------------------------------------\n");
  printf(" 3 -> Buscar comentario sobre a proxima aula\n");
  printf("-------------------------------------------------------\n\n");

  printf("Digite o codigo da disciplina desejada:\n");
  get_input(search_code, sizeof(search_code));

	write_buffer(sockfd, search_code);

	printf("\n");
	read_buffer(sockfd, result);
	printf("%s", result);
	printf("********************************************************\n");
}

// Dado o código de uma disciplina, retornar todas as informações desta disciplina;
void get_full_info(int sockfd)
{
	char result[2500];
  char search_code[10];

  printf("\n-------------------------------------------------------\n");
  printf(" 4 -> Listar informacoes de uma disciplina\n");
  printf("-------------------------------------------------------\n\n");

  printf("Digite o codigo da disciplina desejada:\n");

	get_input(search_code, sizeof(search_code));
	write_buffer(sockfd, search_code);

	printf("\n");
	read_buffer(sockfd, result);
	printf("%s", result);
	printf("********************************************************\n");
}

// Listar todas as informações de todas as disciplinas
void get_all_info(int sockfd)
{
	char result[2500];

  printf("\n-------------------------------------------------------\n");
  printf(" 5 -> Listar informacoes de todas as disciplinas\n");
  printf("-------------------------------------------------------\n\n");

	read_buffer(sockfd, result);
	printf("%s", result);
	printf("********************************************************\n");
}

// *********** Operacoes do PROFESSOR *********** //

// Escrever um texto de comentário sobre a próxima aula de uma disciplina (apenas usuário professor)
void write_comment(int sockfd)
{
  char search_code[10], comment[500];

  printf("\n-------------------------------------------------------\n");
  printf(" 6 -> Escrever comentario sobre a proxima aula de uma disciplina\n");
  printf("-------------------------------------------------------\n\n");

  printf("Digite o codigo da disciplina desejada:\n");

	get_input(search_code, sizeof(search_code));
	write_buffer(sockfd, search_code);

  printf("\nDigite o comentario que deseja inserir em %s:\n", search_code);

	get_input(comment, sizeof(comment));
	write_buffer(sockfd, comment);

	printf("\n*** Comentario adicionado!! ***\n");
	printf("\n********************************************************\n");
}

// *********** Prints *********** //

void print_tela_inicial()
{
	printf("\n-------------------------------------------------------\n");
	printf("Opçoes de login disponiveis:\n");
	printf("-------------------------------------------------------\n\n");

	printf("1. Login Professor\n");
	printf("2. Login Aluno\n");
	printf("0. Exit\n");
	printf("\n-------------------------------------------------------\n\n");
}

void print_ops_professor()
{
	printf("\n-------------------------------------------------------\n");
	printf("Operacoes disponiveis:\n");
	printf("-------------------------------------------------------\n\n");
	printf(" 0. Logout\n");
	printf(" 1. Listar codigos das disciplinas\n");
	printf(" 2. Buscar ementa\n");
	printf(" 3. Buscar comentario sobre a proxima aula\n");
	printf(" 4. Listar informacoes de uma disciplina\n");
	printf(" 5. Listar informacoes de todas as disciplinas\n");
	printf(" 6. Escrever comentario sobre a proxima aula de uma disciplina\n");
	printf("\n-------------------------------------------------------\n\n");
}

void print_ops_aluno()
{
	printf("\n-------------------------------------------------------\n");
	printf("Operacoes disponiveis:\n");
	printf("-------------------------------------------------------\n\n");
	printf(" 0. Logout\n");
	printf(" 1. Listar codigos das disciplinas\n");
	printf(" 2. Buscar ementa\n");
	printf(" 3. Buscar comentario sobre a proxima aula\n");
	printf(" 4. Listar informacoes de uma disciplina\n");
	printf(" 5. Listar informacoes de todas as disciplinas\n");
	printf("\n-------------------------------------------------------\n\n");
}

// *********** Time Evaluation *********** //

int timeval_subtract(TIME *result, TIME *x, TIME *y)
{
  struct timeval xx = *x;
  struct timeval yy = *y;
  x = &xx; y = &yy;

  if (x->tv_usec > 999999)
  {
    x->tv_sec += x->tv_usec / 1000000;
    x->tv_usec %= 1000000;
  }

  if (y->tv_usec > 999999)
  {
    y->tv_sec += y->tv_usec / 1000000;
    y->tv_usec %= 1000000;
  }

  result->tv_sec = x->tv_sec - y->tv_sec;

  if ((result->tv_usec = x->tv_usec - y->tv_usec) < 0)
  {
    result->tv_usec += 1000000;
    result->tv_sec--; // borrow
  }

  return result->tv_sec < 0;
}

void communication_time_eval(int sockfd)
{
	TIME sent, received, diff;
	char buffer[MAXDATASIZE];
	int i;
	FILE *f = fopen("../logs/time_log/client.txt", "a");

	if (f == NULL)
	{
    	printf("Error opening file!\n");
    	exit(1);
	}

	for(i=0; i<98; i++)
	{
		buffer[i] = 'a';
	}
	buffer[i] = '\0';

	gettimeofday(&sent, NULL);
	send(sockfd, buffer, MAXDATASIZE, 0);

	int num = recv(sockfd, buffer, MAXDATASIZE, 0);
	if (num < 0)
	{
		perror("ERROR: Reading from socket didnt go well..");
		exit(0);
	}
	gettimeofday(&received, NULL);

	if(!timeval_subtract(&diff, &received, &sent))
	{
			fprintf(f, "%ld.%06ld\n", diff.tv_sec, diff.tv_usec);
	}
	fclose(f);
}

void function_time_eval(void (*operation)(int), int sockfd, int opcode)
{
	TIME before, after, diff;
	char filename[50];

	sprintf(filename, "../logs/time_log/operation_%d", opcode);
	strcat(filename, ".txt");

	FILE *f = fopen(filename, "a");
	if (f == NULL)
	{
    	printf("Error opening file!\n");
    	exit(1);
	}

	gettimeofday(&before, NULL);
	(*operation)(sockfd);
	gettimeofday(&after, NULL);

	if(!timeval_subtract(&diff, &after, &before))
	{
			fprintf(f, "%ld.%06ld\n", diff.tv_sec, diff.tv_usec);
	}
	fclose(f);
}
