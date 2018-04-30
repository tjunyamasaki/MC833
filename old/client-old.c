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
int get_input(char *input);

// Funcoes para comunicacao
void write_buffer(int sockfd, char *msg, int msglen);
void read_buffer(int sockfd, char *msg, int msglen);


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

// ****************** Prints ****************** //

void print_tela_inicial();
void print_ops_professor();
void print_ops_aluno();

void print_results(int sockfd);

// ****************** Time evaluation for communication ****************** //
int timeval_subtract(TIME *result, TIME *x, TIME *y);
void communication_time_eval(int sockfd);
void function_time_eval(void (*operation)(int), int sockfd, int opcode);

// ****************** MAIN CODE ****************** //

int main(int argc, char *argv[])
{
	int sockfd;
	char buf[MAXDATASIZE];
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];

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
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1)
				{
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

	if (p == NULL)
	{
		fprintf(stderr, "Client: failed to connect\n");
		return 2;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
			s, sizeof s);
	printf("Client: connecting to %s\n", s);

	freeaddrinfo(servinfo); // All done with this structure

	int login = 1;
	while(login)
	{
		print_tela_inicial();

		printf("Selecione uma opcao:\n");
		scanf("%d", &login);

		switch(login)
		{
			case 1: // Professor
				// Envia codigo do login de professor
				write_buffer(sockfd, "1", 1);
				printf("\nBem Vindo Professor!\n");
				professor(sockfd, buf);
				printf("Professor Logging out...\n");
			break;

			case 2: // Aluno
				// Envia codigo do login de aluno
				write_buffer(sockfd, "2", 1);
				printf("\nBem Vindo Aluno!\n");
				aluno(sockfd, buf);
				printf("Aluno Logging out...\n");
			break;

			case 0: // Exit
				write_buffer(sockfd, "0", 1);
			break;

			default:
				printf("Operacao Inval¡da.\n");
		}
	}

	// -------------------------------------------------------------- //
	// ------------------------ MAIN CLIENT CODE -------------------- //
	// -------------------------------------------------------------- //

	close(sockfd);
	return 0;
}

// ******************* Project related functions ******************** //

void professor(int sockfd, char *buf)
{
	int num = 1;
	while(num)
	{
		print_ops_professor();

		int choice;

		printf("Selecione uma operacao:\n");
		scanf("%d", &choice);

		char opcode[12];
		sprintf(opcode, "%d", choice);

		if(choice)
		{
			if(choice <= 7 && choice > 0) {
				// Envia OP Code
				write_buffer(sockfd, opcode, 1);

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
						communication_time_eval(sockfd); // Funcao 'escondida'. Usada apenas para time evaluation.
						break;
				}
			}
			else
			{
				printf("Invalid Op Code!.\n");
			}
		}
		else
		{
			num = choice;
			write_buffer(sockfd, opcode, 1);
		}
	}
}

void aluno(int sockfd, char *buf)
{
int choice, size;

do {
	print_ops_aluno();



	printf("Selecione uma operacao:\n");
	//scanf("%d", &choice);

	char opcode[12];
	size = get_input(opcode);
	//sprintf(opcode, "%d", choice);
	choice = atoi(opcode);
	// Envia OP Code
	write_buffer(sockfd, opcode, size);

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
		default:
			printf("Invalid Op Code!.\n");
	}
} while(choice);


	while()
	{

	}
}

// ************** [Server] - Funcoes Basicas ********************** //

// Get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

// Previne inputs invalidos
int get_input(char *input)
{
	fgets(input, sizeof(input), stdin);

	input[strcspn(input, "\r\n")] = '\0';

	return strlen(input);
}

// ***************** WRITE AND READ SOCKET ***************** //

void write_buffer(int sockfd, char *msg, int msglen)
{
	int num = send(sockfd, msg, msglen, 0);

	if (num < 0) {
		perror("ERROR: Writing to socket didnt go well..");
		exit(0);
	}
}

void read_buffer(int sockfd, char *buffer, int bufferlen){

	int num = recv(sockfd, buffer, bufferlen, 0);

	if (num < 0) {
		perror("ERROR: Reading from socket didnt go well..");
		exit(0);
	}
}

// *********************** Operacoes ALUNO/PROFESSOR *********************** //

// Listar todos os códigos de disciplinas com seus respectivos títulos;
void list_codes(int sockfd)
{
  printf("\n---------------------------------------\n");
  printf(" 1 -> Listar codigos das disciplinas\n");
  printf("---------------------------------------\n\n");

  print_results(sockfd);
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

	send(sockfd, search_code, 6, 0);

	printf("\n");
	print_results(sockfd);
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

	send(sockfd, search_code, 6, 0);

	printf("\n");
	print_results(sockfd);
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

	send(sockfd, search_code, 6, 0);

	print_results(sockfd);
}

// Listar todas as informações de todas as disciplinas
void get_all_info(int sockfd)
{
  printf("\n---------------------------------------\n");
  printf(" 5 -> Listar informacoes de todas as disciplinas\n");
  printf("---------------------------------------\n\n");

	print_results(sockfd);
}

// *********************** Operacoes do PROFESSOR *********************** //

// Escrever um texto de comentário sobre a próxima aula de uma disciplina (apenas usuário professor)
void write_comment(int sockfd)
{
  char search_code[7], comment[500];

  printf("\n---------------------------------------\n");
  printf(" 6 -> Escrever comentario sobre a proxima aula de uma disciplina\n");
  printf("---------------------------------------\n\n");

  printf("Digite o codigo da disciplina desejada:\n");
  scanf("%s", search_code);

	send(sockfd, search_code, 6, 0);

  printf("\nDigite o comentario que deseja inserir em %s:\n", search_code);
	scanf(" ");
  fgets(comment, sizeof(comment), stdin);

	comment[strcspn(comment, "\r\n")] = '\0';

	write_buffer(sockfd, comment, strlen(comment));

	printf("\nComentario adicionado!!\n");

  printf("\n---------------------------------------\n\n");
}

// ***************************** Prints ***************************** //

void print_tela_inicial()
{
	printf("\n--------------------------------------- \n");
	printf("Choose one of the operations below:\n");
	printf("--------------------------------------- \n\n");

	printf("1. Login Professor\n");
	printf("2. Login Aluno\n");
	printf("0. Exit\n");
	printf("\n--------------------------------------- \n\n");
}

void print_ops_professor()
{
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

void print_ops_aluno()
{
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

void print_results(int sockfd)
{
	// Lendo tamanho da frase
	char tamemstring[10];

	read_buffer(sockfd, tamemstring, sizeof(int));

	int tam = atoi(tamemstring);

	char frase[MAXDATASIZE];

	for (int i = 0; i < tam; i = i+MAXDATASIZE) {
		read_buffer(sockfd, frase, MAXDATASIZE);
		printf("%s", frase);
	}
}

// ***************************** Time Evaluation ***************************** //

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
	FILE *f = fopen("time_log/client.txt", "a");

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
	char filename[15];

	sprintf(filename, "time_log/operation_%d", opcode);
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
