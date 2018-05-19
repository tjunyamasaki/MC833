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

// Ip and port struct
typedef struct sockaddr_in ADDRESS;

// Esrutura do datagrama recebido
typedef struct parameters
{
	char usercode[3];
	char opcode[3];
	char search_code[7];
	char comment[500];
} Message;

#define SERVERPORT 8000 // Porta a qual o cliente se conecta
#define MAXDATASIZE 3000 // Numero maximo de bytes que sao recebidos em um pacote
#define MAXBUFLEN 520

// ************** [Client/Server] - Basic functions ************** //

// Funcao para lidar com entrada
void get_input(char *input, size_t maxlen);

// Funcoes para comunicacao
void write_buffer(int sockfd, ADDRESS *their_addr, Message *msg);
void read_buffer(int sockfd, ADDRESS *their_addr, char *msg);

// ******************* Project related functions ******************** //

// Interface de login
void login(int sockfd, ADDRESS *their_addr);

// Interfaces do aluno e professor
void professor(int sockfd, ADDRESS *their_addr, Message *msg);
void aluno(int sockfd, ADDRESS *their_addr, Message *msg);

// Operacoes dos alunos/professores
void list_codes(int sockfd, ADDRESS *their_addr, Message *msg);
void get_ementa(int sockfd, ADDRESS *their_addr, Message *msg);
void get_comment(int sockfd, ADDRESS *their_addr, Message *msg);
void get_full_info(int sockfd, ADDRESS *their_addr, Message *msg);
void get_all_info(int sockfd, ADDRESS *their_addr, Message *msg);

// Operacoes dos professores
void write_comment(int sockfd, ADDRESS *their_addr, Message *msg);

// ****************** Prints ****************** //

void print_tela_inicial();
void print_ops_professor();
void print_ops_aluno();

// ****************** Time evaluation for communication ****************** //
int timeval_subtract(TIME *result, TIME *x, TIME *y);
void communication_time_eval(int sockfd);
void function_time_eval(void (*operation)(int, ADDRESS*, Message*), int sockfd, ADDRESS *their_addr, Message *msg);

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

	login(sockfd, &their_addr);  // Inicia 'aplicacao'

	close(sockfd);

	return 0;
}

// ********************************** FUNCOES ********************************** //

// ***************** [Server/Client] - Funcoes Basicas ***************** //

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

void write_buffer(int sockfd, ADDRESS *their_addr, Message *msg)
{
	int numbytes;
	char buf[MAXBUFLEN];
	socklen_t addr_len;

	addr_len = sizeof(struct sockaddr);

	memset(buf,0,strlen(buf));

	printf("BUFFER: %s\n", buf);
	printf("msg usercode: %s\n", msg->usercode);
	printf("msg opcode: %s\n", msg->opcode);
	printf("msg search_code: %s\n", msg->search_code);
	printf("msg comment: %s\n", msg->comment);

	strcat(buf, msg->usercode);
	strcat(buf, msg->opcode);
	strcat(buf, msg->search_code);
	strcat(buf, msg->comment);

	numbytes = sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *)their_addr, addr_len);

	if (numbytes == -1)
	{
		perror("\nERROR: Writing datagram didnt go well...\n");
		exit(0);
	}
}

void read_buffer(int sockfd, ADDRESS *their_addr, char *msg)
{
	int numbytes;
	char buf[MAXBUFLEN];
	socklen_t addr_len;

	addr_len = sizeof(struct sockaddr);

	numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0, (struct sockaddr *)their_addr, &addr_len);

	if (numbytes == -1)
	{
			perror("ERROR: Receiving datagram");
			exit(1);
	}

	// printf("got packet from %s\n", inet_ntoa(their_addr->sin_addr));
	// printf("packet is %d bytes long\n",numbytes);
	buf[numbytes] = '\0';
	// printf("packet contains \n\n %s \n",buf);

	strncpy(msg, buf, numbytes+1);  // Copia apenas msg que deveria ser recebida
}

// ******************* Funcoes relacionadas ao projeto ******************** //

// *********** Interfaces *********** //

// Realiza login no servidor.
void login(int sockfd, ADDRESS *their_addr)
{
	int login;
	Message msg;

	printf("\n********************************************************\n");
	printf("\t BEM VINDO AO BANCO DE DISCIPLINAS!!\n");
	printf("********************************************************\n");

	do {
		print_tela_inicial();
		printf("Selecione uma opcao:\n");

		//Limpar campos
		memset(msg.usercode,0,strlen(msg.usercode));
		memset(msg.search_code,0,strlen(msg.search_code));
		memset(msg.opcode,0,strlen(msg.opcode));
		memset(msg.comment,0,strlen(msg.comment));

		get_input(msg.usercode, sizeof(msg.usercode));

		login = atoi(msg.usercode);
		switch(login)
		{
			case 1: // Professor
				professor(sockfd, their_addr, &msg);
				break;
			case 2: // Aluno
				aluno(sockfd, their_addr, &msg);
				break;
			case 0: // Exit
				printf("\nEncerrando login.\n");
				break;
			default:
				printf("\nOperacao Inval¡da.\n");
		}
	} while(login);
}

void professor(int sockfd, ADDRESS *their_addr, Message *msg)
{
	int choice;

	printf("\n-------------------------------------------------------\n");
	printf("\n\t\t*** Bem Vindo Professor! ***\n");

	print_ops_professor();

	printf("Selecione uma operacao:\n");

	get_input(msg->opcode, sizeof(msg->opcode));

	choice = atoi(msg->opcode);
	switch (choice)
	{
		case 1:
			function_time_eval(list_codes, sockfd, their_addr, msg);
			break;
		case 2:
			function_time_eval(get_ementa, sockfd, their_addr, msg);
			break;
		case 3:
			function_time_eval(get_comment, sockfd, their_addr, msg);
			break;
		case 4:
			function_time_eval(get_full_info, sockfd, their_addr, msg);
			break;
		case 5:
			function_time_eval(get_all_info, sockfd, their_addr, msg);
			break;
		case 6:
			function_time_eval(write_comment, sockfd, their_addr, msg);
			break;
		case 0:
				printf("\nProfessor logging out...\n");
				break;
		default:
			printf("\nInvalid Op Code!\n");
		}
}

void aluno(int sockfd, ADDRESS *their_addr, Message *msg)
{
	int choice;

	printf("\n-------------------------------------------------------\n");
	printf("\n\t\t*** Bem Vindo Aluno! ***\n");

	print_ops_aluno();

	printf("Selecione uma operacao:\n");
	get_input(msg->opcode, sizeof(msg->opcode));

	choice = atoi(msg->opcode);

	switch (choice)
	{
		case 1:
			function_time_eval(list_codes, sockfd, their_addr, msg);
			break;
		case 2:
			function_time_eval(get_ementa, sockfd, their_addr, msg);
			break;
		case 3:
			function_time_eval(get_comment, sockfd, their_addr, msg);
			break;
		case 4:
			function_time_eval(get_full_info, sockfd, their_addr, msg);
			break;
		case 5:
			function_time_eval(get_all_info, sockfd, their_addr, msg);
			break;
		case 0:
			printf("\nAluno logging out...\n");
			break;
		default:
			printf("\nInvalid Op Code.\n");
	}
}

// *********** Operacoes de ALUNO e PROFESSOR *********** //

// Listar todos os códigos de disciplinas com seus respectivos títulos;
void list_codes(int sockfd, ADDRESS *their_addr, Message *msg)
{
	char result[2500];

	// Envia OP Code
	write_buffer(sockfd, their_addr, msg);

  printf("\n-------------------------------------------------------\n");
  printf(" 1 -> Listar codigos das disciplinas\n");
  printf("-------------------------------------------------------\n\n");

	// Recebe Resultado
	read_buffer(sockfd, their_addr, result);
	printf("%s", result);
	printf("********************************************************\n");
}

// Dado o código de uma disciplina, retornar a ementa;
void get_ementa(int sockfd, ADDRESS *their_addr, Message *msg)
{
	char result[2500];

  printf("\n-------------------------------------------------------\n");
  printf(" 2 -> Buscar ementa\n");
  printf("-------------------------------------------------------\n\n");

  printf("Digite o codigo da disciplina desejada:\n");
	get_input(msg->search_code, sizeof(msg->search_code));

	// strcat(msg_buf, search_code);

	write_buffer(sockfd, their_addr, msg);

	printf("\n");
	read_buffer(sockfd, their_addr, result);
	printf("%s", result);
	printf("********************************************************\n");
}

// Dado o código de uma disciplina, retornar o texto de comentário sobre a próxima aula.
void get_comment(int sockfd, ADDRESS *their_addr, Message *msg)
{
	char result[2500];

  printf("\n-------------------------------------------------------\n");
  printf(" 3 -> Buscar comentario sobre a proxima aula\n");
  printf("-------------------------------------------------------\n\n");

  printf("Digite o codigo da disciplina desejada:\n");
  get_input(msg->search_code, sizeof(msg->search_code));

	// strcat(msg, search_code);

	write_buffer(sockfd, their_addr, msg);

	printf("\n");
	read_buffer(sockfd, their_addr, result);
	printf("%s", result);
	printf("********************************************************\n");
}

// Dado o código de uma disciplina, retornar todas as informações desta disciplina;
void get_full_info(int sockfd, ADDRESS *their_addr, Message *msg)
{
	char result[3000];

  printf("\n-------------------------------------------------------\n");
  printf(" 4 -> Listar informacoes de uma disciplina\n");
  printf("-------------------------------------------------------\n\n");

  printf("Digite o codigo da disciplina desejada:\n");

	get_input(msg->search_code, sizeof(msg->search_code));

	// strcat(msg_buf, search_code);

	write_buffer(sockfd, their_addr, msg);

	printf("\n");
	read_buffer(sockfd, their_addr, result);
	printf("%s", result);
	printf("********************************************************\n");
}

// Listar todas as informações de todas as disciplinas
void get_all_info(int sockfd, ADDRESS *their_addr, Message *msg)
{
	char result[3000];

	// Envia OP Code
	write_buffer(sockfd, their_addr, msg);

  printf("\n-------------------------------------------------------\n");
  printf(" 5 -> Listar informacoes de todas as disciplinas\n");
  printf("-------------------------------------------------------\n\n");

	read_buffer(sockfd, their_addr, result);
	printf("%s", result);
	printf("********************************************************\n");
}

// *********** Operacoes do PROFESSOR *********** //

// Escrever um texto de comentário sobre a próxima aula de uma disciplina (apenas usuário professor)
void write_comment(int sockfd, ADDRESS *their_addr, Message *msg)
{
  char response[6];
	int result;

  printf("\n-------------------------------------------------------\n");
  printf(" 6 -> Escrever comentario sobre a proxima aula de uma disciplina\n");
  printf("-------------------------------------------------------\n\n");

  printf("Digite o codigo da disciplina desejada:\n");

	get_input(msg->search_code, sizeof(msg->search_code));
	// strcat(msg_buf, search_code);

  printf("\nDigite o comentario que deseja inserir em %s:\n", msg->search_code);

	get_input(msg->comment, sizeof(msg->comment));
	// strcat(msg_buf, comment);

	write_buffer(sockfd, their_addr, msg);

	read_buffer(sockfd, their_addr, response);
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

void function_time_eval(void (*operation)(int, ADDRESS*, Message*), int sockfd, ADDRESS *their_addr, Message *msg)
{
	TIME before, after, diff;
	char filename[50] = "time_log/client_operation_";

	strcat(filename, msg->opcode);
	strcat(filename, ".txt");

	FILE *f = fopen(filename, "a");
	if (f == NULL)
	{
    	printf("Error opening file!\n");
    	exit(1);
	}

	gettimeofday(&before, NULL);
	(*operation)(sockfd, their_addr, msg);
	gettimeofday(&after, NULL);

	if(!timeval_subtract(&diff, &after, &before))
	{
			fprintf(f, "%ld.%06ld\n", diff.tv_sec, diff.tv_usec);
	}
	fclose(f);
}
