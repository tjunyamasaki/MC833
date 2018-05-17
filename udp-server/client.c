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
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

// Handling time
#include <sys/time.h>
typedef struct timeval TIME;

#define SERVERPORT 8000 // Porta a qual o cliente se conecta
#define MAXDATASIZE 100 // Numero maximo de bytes que sao enviados em um pacote
#define MAXBUFLEN 500

// ************** [Client/Server] - Basic functions ************** //

// Get sockaddr, IPv4 or IPv6
void *get_in_addr(struct sockaddr *sa);

// Funcao para lidar com entrada
void get_input(char *input, size_t maxlen);

// Funcoes para comunicacao
void write_buffer(int sockfd, char *msg);
void read_buffer(int sockfd, char *msg, struct sockaddr_in their_addr);

// ******************* Project related functions ******************** //

// Interface de login
void login(int sockfd);

// Interfaces do aluno e professor
void professor(int sockfd, char *usercode);
void aluno(int sockfd, char *usercode);

// Operacoes dos alunos/professores
void list_codes(int sockfd, char *msg_buf);
void get_ementa(int sockfd, char *msg_buf);
void get_comment(int sockfd, char *msg_buf);
void get_full_info(int sockfd, char *msg_buf);
void get_all_info(int sockfd, char *msg_buf);

// Operacoes dos professores
void write_comment(int sockfd, char *msg_buf);

// ****************** Prints ****************** //

void print_tela_inicial();
void print_ops_professor();
void print_ops_aluno();

// ****************** Time evaluation for communication ****************** //
int timeval_subtract(TIME *result, TIME *x, TIME *y);
void communication_time_eval(int sockfd);
void function_time_eval(void (*operation)(int, char[1000]), int sockfd, char msg_buf[1000], char opcode[5]);

// ****************** MAIN CODE ****************** //

int main(int argc, char *argv[])
{
	int sockfd;
  struct sockaddr_in their_addr; // Connector’s address information
  struct hostent *he;

	if (argc != 2)
	{
	    fprintf(stderr,"Usage: ./client hostname\n");
	    exit(1);
	}

	if ((he=gethostbyname(argv[1])) == NULL)				 // Get the host info
	{
			perror("gethostbyname");
			exit(1);
	}

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{
			perror("socket");
			exit(1);
	}

	their_addr.sin_family = AF_INET;     // Host byte order
	their_addr.sin_port = htons(SERVERPORT); // Short, network byte order
	their_addr.sin_addr = *((struct in_addr *)he->h_addr);
	memset(&(their_addr.sin_zero), '\0', 8);  // Zero the rest of the struct

	printf("\n-------------------------------------------------------\n");
	printf("Client: sending to %s\n", inet_ntoa(their_addr.sin_addr));
	printf("-------------------------------------------------------\n");

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


//   					NOVA ESTRUTURA DO BUFFER (A PRINCIPIO)  //
//   ***************************************************************************************************************
//   * 6 bytes header  *  2 bytes usercode *  2 bytes opcode   *  8 bytes codigo busca    *   200 bytes comentario *
//   ***************************************************************************************************************


// *********** WRITE AND READ SOCKET *********** //

void write_buffer(int sockfd, char *msg)
{
	int bytesleft, numbytes;
	char header[6], *auxmsg;

	bytesleft =  strlen(msg);
	sprintf(header, "%d", bytesleft);

	numbytes = send(sockfd, header, 6, 0);
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

void read_buffer(int sockfd, struct sockaddr_in their_addr, char *msg)
{
	char header[6], *workbuffer;
	int numbytes;
	char buf[MAXBUFLEN];
	socklen_t addr_len;


	addr_len = sizeof(struct sockaddr);
	if ((numbytes=recvfrom(sockfd, buf, MAXBUFLEN-1 , 0, (struct sockaddr *)&their_addr, &addr_len)) == -1)
	{
			perror("recvfrom");
			exit(1);
	}

	printf("got packet from %s\n", inet_ntoa(their_addr.sin_addr));
	printf("packet is %d bytes long\n",numbytes);
	buf[numbytes] = '\0';
	printf("packet contains \"%s\"\n",buf);

	strncpy(msg, buf, numbytes+1);  // Copia apenas msg que deveria ser recebida
}

// ******************* Funcoes relacionadas ao projeto ******************** //

// *********** Interfaces *********** //

// Realiza login no servidor.
void login(int sockfd)
{
	int login;
	char input[5];

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
				professor(sockfd, input);
				break;
			case 2: // Aluno
				aluno(sockfd, input);
				break;
			case 0: // Exit
				printf("\nFechando conexao.\n");
				break;
			default:
				printf("\nOperacao Inval¡da.\n");
		}
	} while(login);
}

void professor(int sockfd, char *usercode)
{
	int choice;
	char msg_buf[1000], opcode[5];

	printf("\n-------------------------------------------------------\n");
	printf("\n\t\t*** Bem Vindo Professor! ***\n");


	do {
		strcpy(msg_buf, usercode);

		print_ops_professor();

		printf("Selecione uma operacao:\n");

		get_input(opcode, sizeof(opcode));

		strcat(msg_buf, opcode);

		choice = atoi(opcode);

		switch (choice)
		{
			case 1:
				function_time_eval(list_codes, sockfd, msg_buf, opcode);
				break;
			case 2:
				function_time_eval(get_ementa, sockfd, msg_buf, opcode);
				break;
			case 3:
				function_time_eval(get_comment, sockfd, msg_buf, opcode);
				break;
			case 4:
				function_time_eval(get_full_info, sockfd, msg_buf, opcode);
				break;
			case 5:
				function_time_eval(get_all_info, sockfd, msg_buf, opcode);
				break;
			case 6:
				function_time_eval(write_comment, sockfd, msg_buf, opcode);
				break;
			case 0:
					printf("\nProfessor logging out...\n");
					break;
			default:
				printf("\nInvalid Op Code!\n");
			}
	} while(choice);
}

void aluno(int sockfd, char *usercode)
{
	int choice;
	char msg_buf[1000], opcode[6];

	printf("\n-------------------------------------------------------\n");
	printf("\n\t\t*** Bem Vindo Aluno! ***\n");

	do {
		strcpy(msg_buf, usercode);

		print_ops_aluno();

		printf("Selecione uma operacao:\n");
		get_input(opcode, sizeof(opcode));

		strcat(msg_buf, opcode);

		choice = atoi(opcode);

		switch (choice)
		{
			case 1:
				function_time_eval(list_codes, sockfd, msg_buf, opcode);
				break;
			case 2:
				function_time_eval(get_ementa, sockfd, msg_buf, opcode);
				break;
			case 3:
				function_time_eval(get_comment, sockfd, msg_buf, opcode);
				break;
			case 4:
				function_time_eval(get_full_info, sockfd, msg_buf, opcode);
				break;
			case 5:
				function_time_eval(get_all_info, sockfd, msg_buf, opcode);
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
void list_codes(int sockfd, char *msg_buf)
{
	char result[2500];

	// Envia OP Code
	write_buffer(sockfd, msg_buf);

  printf("\n-------------------------------------------------------\n");
  printf(" 1 -> Listar codigos das disciplinas\n");
  printf("-------------------------------------------------------\n\n");

	// Recebe Resultado
	read_buffer(sockfd, result);
	printf("%s", result);
	printf("********************************************************\n");
}

// Dado o código de uma disciplina, retornar a ementa;
void get_ementa(int sockfd, char *msg_buf)
{
	char result[2500];
  char search_code[10];

  printf("\n-------------------------------------------------------\n");
  printf(" 2 -> Buscar ementa\n");
  printf("-------------------------------------------------------\n\n");

  printf("Digite o codigo da disciplina desejada:\n");
	get_input(search_code, sizeof(search_code));

	strcat(msg_buf, search_code);

	write_buffer(sockfd, msg_buf);

	printf("\n");
	read_buffer(sockfd, result);
	printf("%s", result);
	printf("********************************************************\n");
}

// Dado o código de uma disciplina, retornar o texto de comentário sobre a próxima aula.
void get_comment(int sockfd, char *msg_buf)
{
	char result[2500];
  char search_code[10];

  printf("\n-------------------------------------------------------\n");
  printf(" 3 -> Buscar comentario sobre a proxima aula\n");
  printf("-------------------------------------------------------\n\n");

  printf("Digite o codigo da disciplina desejada:\n");
  get_input(search_code, sizeof(search_code));

	strcat(msg_buf, search_code);

	write_buffer(sockfd, msg_buf);

	printf("\n");
	read_buffer(sockfd, result);
	printf("%s", result);
	printf("********************************************************\n");
}

// Dado o código de uma disciplina, retornar todas as informações desta disciplina;
void get_full_info(int sockfd, char *msg_buf)
{
	char result[2500];
  char search_code[10];

  printf("\n-------------------------------------------------------\n");
  printf(" 4 -> Listar informacoes de uma disciplina\n");
  printf("-------------------------------------------------------\n\n");

  printf("Digite o codigo da disciplina desejada:\n");

	get_input(search_code, sizeof(search_code));

	strcat(msg_buf, search_code);

	write_buffer(sockfd, msg_buf);

	printf("\n");
	read_buffer(sockfd, result);
	printf("%s", result);
	printf("********************************************************\n");
}

// Listar todas as informações de todas as disciplinas
void get_all_info(int sockfd, char *msg_buf)
{
	char result[2500];

	// Envia OP Code
	write_buffer(sockfd, msg_buf);

  printf("\n-------------------------------------------------------\n");
  printf(" 5 -> Listar informacoes de todas as disciplinas\n");
  printf("-------------------------------------------------------\n\n");

	read_buffer(sockfd, result);
	printf("%s", result);
	printf("********************************************************\n");
}

// *********** Operacoes do PROFESSOR *********** //

// Escrever um texto de comentário sobre a próxima aula de uma disciplina (apenas usuário professor)
void write_comment(int sockfd, char *msg_buf)
{
  char search_code[10], comment[500], response[6];
	int result;

  printf("\n-------------------------------------------------------\n");
  printf(" 6 -> Escrever comentario sobre a proxima aula de uma disciplina\n");
  printf("-------------------------------------------------------\n\n");

  printf("Digite o codigo da disciplina desejada:\n");

	get_input(search_code, sizeof(search_code));
	strcat(msg_buf, search_code);

  printf("\nDigite o comentario que deseja inserir em %s:\n", search_code);

	get_input(comment, sizeof(comment));
	strcat(msg_buf, comment);

	write_buffer(sockfd, msg_buf);

	read_buffer(sockfd, response);
	result = atoi(response);
	if(result)
	{
		printf("\n*** Comentario adicionado!! ***\n");
	}
	else
	{
		printf("\n*** Falha ao tentar adicionar comentario! ***\n");
	}

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

void function_time_eval(void (*operation)(int, char[1000]), int sockfd, char msg_buf[1000], char opcode[5])
{
	TIME before, after, diff;
	char filename[50] = "time_log/client_operation_";

	strcat(filename, opcode);
	strcat(filename, ".txt");

	FILE *f = fopen(filename, "a");
	if (f == NULL)
	{
    	printf("Error opening file!\n");
    	exit(1);
	}

	gettimeofday(&before, NULL);
	(*operation)(sockfd, msg_buf);
	gettimeofday(&after, NULL);

	if(!timeval_subtract(&diff, &after, &before))
	{
			fprintf(f, "%ld.%06ld\n", diff.tv_sec, diff.tv_usec);
	}
	fclose(f);
}
