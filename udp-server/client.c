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
#define MAXDATASIZE 3000  // Tamanho maximo para mensagem recebida
#define MAXBUFLEN 513 // Numero maximo de bytes que sao enviados em um pacote

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
void professor(int sockfd, ADDRESS *their_addr);
void aluno(int sockfd, ADDRESS *their_addr);

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
//   * 3 bytes usercode *  3 bytes opcode   *  7 bytes codigo busca    *   500 bytes comentario *
//   ***************************************************************************************************************


// *********** WRITE AND READ SOCKET *********** //

void write_buffer(int sockfd, ADDRESS *their_addr, Message *msg)
{
	int numbytes;
	char *buf;
	socklen_t addr_len;

	buf = calloc(MAXBUFLEN, sizeof(char));

	addr_len = sizeof(struct sockaddr);

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
	char *buf;
	socklen_t addr_len;

	buf = calloc(MAXDATASIZE, sizeof(char));

	addr_len = sizeof(struct sockaddr);

	numbytes = recvfrom(sockfd, buf, MAXDATASIZE-1 , 0, (struct sockaddr *)their_addr, &addr_len);

	if (numbytes == -1)
	{
			perror("ERROR: Receiving datagram");
			exit(1);
	}

	buf[numbytes] = '\0';

	strncpy(msg, buf, numbytes+1);  // Copia apenas msg que deveria ser recebida

	free(buf);
}

// ******************* Funcoes relacionadas ao projeto ******************** //

// *********** Interfaces *********** //

// Realiza login no servidor.
void login(int sockfd, ADDRESS *their_addr)
{
	int login;
	char usercode[3];

	printf("\n********************************************************\n");
	printf("\t BEM VINDO AO BANCO DE DISCIPLINAS!!\n");
	printf("********************************************************\n");

	do {
		print_tela_inicial();
		printf("Selecione uma opcao:\n");

		//Limpar campos
		// memset(msg.usercode,0,strlen(msg.usercode));
		// memset(msg.search_code,0,strlen(msg.search_code));
		// memset(msg.opcode,0,strlen(msg.opcode));
		// memset(msg.comment,0,strlen(msg.comment));

		// get_input(msg.usercode, sizeof(msg.usercode));

		get_input(usercode, sizeof(usercode));

		login = atoi(usercode);
		switch(login)
		{
			case 1: // Professor
				professor(sockfd, their_addr);
				break;
			case 2: // Aluno
				aluno(sockfd, their_addr);
				break;
			case 0: // Exit
				printf("\nEncerrando login.\n");
				break;
			default:
				printf("\nOperacao Inval¡da.\n");
		}
	} while(login);
}

void professor(int sockfd, ADDRESS *their_addr)
{
	int choice;
	Message *msg;

	msg = calloc(1, sizeof(Message));

	strcpy(msg->usercode, "1");

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
			printf("\nOpcao invalida.\n");
		}
		free(msg);
}

void aluno(int sockfd, ADDRESS *their_addr)
{
	int choice;
	Message *msg;

	msg = calloc(1, sizeof(Message));

	strcpy(msg->usercode, "2");

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
			printf("\nOpcao invalida.\n");
	}
	free(msg);
}

// *********** Operacoes de ALUNO e PROFESSOR *********** //

// Listar todos os códigos de disciplinas com seus respectivos títulos;
void list_codes(int sockfd, ADDRESS *their_addr, Message *msg)
{
	char result[3000];

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
	char result[3000];

  printf("\n-------------------------------------------------------\n");
  printf(" 2 -> Buscar ementa\n");
  printf("-------------------------------------------------------\n\n");

  printf("Digite o codigo da disciplina desejada:\n");

	get_input(msg->search_code, sizeof(msg->search_code));

	if(strlen(msg->search_code) == 5)
	{
		write_buffer(sockfd, their_addr, msg);
	}
	else
	{
		printf("\nCódigo de busca invalido.\n");
		while (getchar() != '\n') { }  // Tira lixo do buffer
		return;
	}

	printf("\n");
	read_buffer(sockfd, their_addr, result);
	printf("%s", result);
	printf("********************************************************\n");
}

// Dado o código de uma disciplina, retornar o texto de comentário sobre a próxima aula.
void get_comment(int sockfd, ADDRESS *their_addr, Message *msg)
{
	char result[3000];

  printf("\n-------------------------------------------------------\n");
  printf(" 3 -> Buscar comentario sobre a proxima aula\n");
  printf("-------------------------------------------------------\n\n");

  printf("Digite o codigo da disciplina desejada:\n");

	printf("sizeof: %d\n", (int) sizeof(msg->search_code));

	get_input(msg->search_code, sizeof(msg->search_code));

	if(strlen(msg->search_code) == 5)
	{
		write_buffer(sockfd, their_addr, msg);
	}
	else
	{
		printf("\nCódigo de busca invalido.\n");
		while (getchar() != '\n') { }  // Tira lixo do buffer
		return;
	}

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

	if(strlen(msg->search_code) <= 5)
	{
		write_buffer(sockfd, their_addr, msg);
	}
	else
	{
		printf("\nCódigo de busca invalido.\n");
		while (getchar() != '\n') { }  // Tira lixo do buffer
		return;
	}

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

	if(strlen(msg->search_code) == 5)
	{
		printf("\nDigite o comentario que deseja inserir em %s:\n", msg->search_code);

		get_input(msg->comment, sizeof(msg->comment));

		write_buffer(sockfd, their_addr, msg);
	}
	else
	{
		printf("\nCódigo de busca invalido.\n");
		while (getchar() != '\n') { }  // Tira lixo do buffer
		return;
	}

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
	printf("Usuarios disponiveis:\n");
	printf("-------------------------------------------------------\n\n");

	printf("1. Professor\n");
	printf("2. Aluno\n");
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
