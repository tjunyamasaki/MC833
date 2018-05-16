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
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

//[MYSQL]
#include <my_global.h>
#include <mysql.h>

// Handling time
#include <sys/time.h>
typedef struct timeval TIME;

#define PORT "8000"  // Porta a qual o cliente se conecta
#define BACKLOG 10	 // Maximo de conexoes pendentes que a fila ira segurar
#define MAXDATASIZE 100 // Numero maximo de bytes que sao enviados em um pacote

// ************** [Client/Server] - Basic functions ************** //

void sigchld_handler(int s); // Get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa);
void write_buffer(int sockfd, char *msg);
int read_buffer(int sockfd, char *usercode, char *opcode, char *search_code, char *comment);

// ****************** [MYSQL] ************************** //

void initError(MYSQL *con);
void finish_with_error(MYSQL *con);
void loginMysql(char *username, char *password, MYSQL *con);
void execute_querry(MYSQL *con, char* querry_command);
void send_results(MYSQL *con, int sockfd);

// ******************* Funcoes especificas do projeto ******************** //

void professor(int sockfd, TIME start_time, char *operation, char *search_code, char *comment);
void aluno(int sockfd, TIME start_time, char *operation, char *search_code);

// Operacoes dos alunos/professores
TIME list_codes(MYSQL *con, int sockfd);
TIME get_ementa(MYSQL *con, int sockfd, char *search_code);
TIME get_comment(MYSQL *con, int sockfd, char *search_code);
TIME get_full_info(MYSQL *con, int sockfd, char *search_code);
TIME get_all_info(MYSQL *con, int sockfd);

// Operacoes dos professores
TIME write_comment(MYSQL *con, int sockfd, char *search_code, char *comment);

// ****************** Avaliacao do tempo para communicacao ****************** //
int timeval_subtract(TIME *result, TIME *x, TIME *y);
void function_time_eval(TIME before, TIME after, char opcode[5]);

// ****************** MAIN CODE ****************** //

int main(void)
{
	int sockfd, new_fd;  // Listen on sock_fd, new connection on new_fd
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage their_addr; // Connector's address information
	socklen_t sin_size;
	struct sigaction sa;
	int yes=1;
	char ip[INET6_ADDRSTRLEN];
	int rv;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0)
	{
		fprintf(stderr, "\nGetaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// Loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next)
	{
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
		{
			perror("\nServer: socket\n");
			continue;
		}

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
		{
			perror("\nSetsockopt\n");
			exit(1);
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
		{
			close(sockfd);
			perror("\nServer: Bind\n");
			continue;
		}

		break;
	}

	freeaddrinfo(servinfo); // All done with this structure

	if (p == NULL)
	{
		fprintf(stderr, "\nServer: Failed to bind\n");
		exit(1);
	}

	if (listen(sockfd, BACKLOG) == -1)
	{
		perror("\nListen\n");
		exit(1);
	}

	sa.sa_handler = sigchld_handler; // Mata todos os processos mortos
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1)
	{
		perror("\nSigaction\n");
		exit(1);
	}

	while(1)
	{
		printf("\n-> Waiting for new connections...\n");

		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		if (new_fd == -1)
		{
			perror("\nAccept\n");
			continue;
		}

		inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), ip, sizeof ip);
		printf("\n*** Server: got connection from %s ***\n", ip);

		if(!fork())
		{
			// -------------------------------------------------------------- //
			// ------------------------ MAIN SERVER CODE -------------------- //
			// -------------------------------------------------------------- //

			close(sockfd); // Child doesn't need the listener

			int user, received;
			char usercode[5], opcode[5], search_code[10], comment[500];
			TIME start_time;

			do {
				printf("\n-> Waiting for request...\n");
				received = read_buffer(new_fd, usercode, opcode, search_code, comment);  // Opcao de login vinda do Client.
				gettimeofday(&start_time, NULL);
				user = atoi(usercode);

				if(received)
				{
					printf("Received login type %d from %s. Processing...\n", user, ip);
					switch(user)
					{
						case 1:
							professor(new_fd, start_time, opcode, search_code, comment);
							break;
						case 2:
							aluno(new_fd, start_time, opcode, search_code);
							break;
						default:
							printf("\nUnexpected login option.\n");
					}
				}
			} while(1);

			close(new_fd);
			printf("\n(%s) Socket closed.\n", ip);
			exit(0);
		}
		close(new_fd);  // Parent doesn't need this
	}
	return 0;
}

// ********************************** FUNCOES ********************************** //

// ***************** [Server/Client] - Funcoes Basicas ***************** //

void sigchld_handler(int s)
{
	(void)s; // Quiet unused variable warning

	// waitpid() might overwrite errno, so we save and restore it:
	int saved_errno = errno;

	while(waitpid(-1, NULL, WNOHANG) > 0);
	errno = saved_errno;
}

// Get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET){
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

// *********** WRITE AND READ SOCKET *********** //

void write_buffer(int sockfd, char *msg)
{
	int bytesleft, numbytes;
	char header[6], *auxmsg;

	bytesleft = strlen(msg);
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

int read_buffer(int sockfd, char *usercode, char *opcode, char *search_code, char *comment)
{
	char header[6], *workbuffer, *auxpointer;
	int numbytes, bytesleft, bytesrcv = 0;

	numbytes = recv(sockfd, header, 6, 0);  // Recebe header

	if (numbytes < 0)
	{
		perror("\nERROR: Reading from socket didnt go well...\n");
		exit(0);
	}
	else if (numbytes == 0)
	{
		printf("\nERROR: Connection closed by Client.\n");
		return 0;
	}

	bytesleft = atoi(header);
	workbuffer = malloc(2*bytesleft * sizeof(char));  // Buffer para tratar msg recebida

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
			printf("\nERROR: Connection closed by Client.\n");
			return 0;
		}

		bytesrcv += numbytes;  // Bytes recebidos ate o momento
		auxpointer += numbytes;  // Atualiza parte do buffer onde inserir msg recebida
		bytesleft -= numbytes;  // Bytes que faltam chegar

	} while(bytesleft > 0);

	workbuffer[bytesrcv] = '\0';

	strncpy(usercode, workbuffer, 2);
	usercode[2] = '\0';
	strncpy(opcode, workbuffer+2, 2);
	opcode[2] = '\0';
	strncpy(search_code, workbuffer+4, 8);
	search_code[8] = '\0';
	strncpy(comment, workbuffer+12, 500);
	comment[500] = '\0';

	free(workbuffer);
	return 1;
}

// *********** [MYSQL] *********** //

void initError(MYSQL *con)
{
	if (con == NULL)
	{
		fprintf(stderr, "%s\n", mysql_error(con));
		exit(1);
	}
}

// Finaliza conexao apresentando erros
void finish_with_error(MYSQL *con)
{
  fprintf(stderr, "%s\n", mysql_error(con));
  mysql_close(con);
  exit(1);
}

void loginMysql(char *username, char *password, MYSQL *con)
{
	if (mysql_real_connect(con, "localhost",
	username, password, "projeto1", 0, NULL, 0) == NULL)
	{
		finish_with_error(con);
	}
}

// Executa querry
void execute_querry(MYSQL *con, char* querry_command)
{
  if (mysql_query(con, querry_command))
	{
      finish_with_error(con);
  }
}

// Envia resultados de uma consulta
void send_results(MYSQL *con, int sockfd)
{
  MYSQL_RES *result = mysql_store_result(con);

  if (result == NULL)
  {
    finish_with_error(con);
  }
  int num_fields = mysql_num_fields(result);

	char table[15000];

	memset(table,0,sizeof(table));

	MYSQL_ROW row;
	MYSQL_FIELD *field;

	strcat(table,"-------------------------------------------------------\n");
	while((field = mysql_fetch_field(result)))
	{
		strcat(table, field->name);
		strcat(table, " | ");
	}
	strcat(table,"\n");
	strcat(table,"-------------------------------------------------------\n");

	while ((row = mysql_fetch_row(result)))
	{
			for(int i = 0; i < num_fields; i++)
			{
					strcat(table, row[i]);
					if(i != num_fields - 1)
					{
						strcat(table, " | ");
					}
					else
					{
						strcat(table, "\n");
					}
			}
	}
	strcat(table,"-------------------------------------------------------\n\n");

	write_buffer(sockfd, table);

	mysql_free_result(result);
}

// ***************** Funcoes relacionadas ao projeto ***************** //

// *********** Interfaces *********** //

void professor(int sockfd, TIME start_time, char *operation, char *search_code, char *comment)
{
	MYSQL *con = mysql_init(NULL);
	initError(con);

	loginMysql("professor", "senha123", con);
	printf("\n *** PROFESSOR Logged In! ***\n");

	int opcode;
	TIME end_time;

	opcode = atoi(operation);
	printf("Received opcode %d. Running...\n", opcode);

	switch(opcode)
	{
		case 1:
			end_time = list_codes(con, sockfd);
			break;
		case 2:
			end_time = get_ementa(con, sockfd, search_code);
			break;
		case 3:
			end_time = get_comment(con, sockfd, search_code);
			break;
		case 4:
			end_time = get_full_info(con, sockfd, search_code);
			break;
		case 5:
			end_time = get_all_info(con, sockfd);
			break;
		case 6:
			end_time = write_comment(con, sockfd, search_code, comment);
			break;
		default:
			printf("\nPedido invalido.\n");
	}

	function_time_eval(start_time, end_time, operation);
	mysql_close(con);  // Closing sql connection
}

void aluno(int sockfd, TIME start_time, char *operation, char *search_code)
{
	MYSQL *con = mysql_init(NULL);
	initError(con);

	loginMysql("aluno", "senha123", con);
	printf("\n*** ALUNO Logged In! ***\n");

	int opcode;
	TIME end_time;

	opcode = atoi(operation);
	printf("\nReceived opcode %d. Running...\n", opcode);

	switch (opcode)
	{
		case 1:
			end_time = list_codes(con, sockfd);
			break;
		case 2:
			end_time = get_ementa(con, sockfd, search_code);
			break;
		case 3:
			end_time = get_comment(con, sockfd, search_code);
			break;
		case 4:
			end_time = get_full_info(con, sockfd, search_code);
			break;
		case 5:
			end_time = get_all_info(con, sockfd);
			break;
		default:
			printf("\nOperacao invalida. \n");
	}
	function_time_eval(start_time, end_time, operation);
	mysql_close(con);  // Closing sql connection
}

// *********** Operacoes de ALUNO e PROFESSOR *********** //

// Listar todos os códigos de disciplinas com seus respectivos títulos;
TIME list_codes(MYSQL *con, int sockfd)
{
  char querry_command[1000];
	TIME end_time;

  strcpy(querry_command, "SELECT CODIGO_DISCIPLINA AS CODIGO, TITULO FROM DISCIPLINAS;");

  execute_querry(con, querry_command);

	gettimeofday(&end_time, NULL);
  send_results(con, sockfd);

	return end_time;
}

// Dado o código de uma disciplina, retornar a ementa;
TIME get_ementa(MYSQL *con, int sockfd, char *search_code)
{
  char querry_command[1000];
	TIME end_time;

	strcpy(querry_command, "SELECT EMENTA FROM DISCIPLINAS WHERE CODIGO_DISCIPLINA = '");
	strcat(querry_command, search_code);
	strcat(querry_command, "';");

	execute_querry(con, querry_command);

	gettimeofday(&end_time, NULL);
	send_results(con, sockfd);

	return end_time;
}

// Dado o código de uma disciplina, retornar o texto de comentário sobre a próxima aula.
TIME get_comment(MYSQL *con, int sockfd, char *search_code)
{
  char querry_command[1000];
	TIME end_time;

	strcpy(querry_command, "SELECT COMENTARIO FROM DISCIPLINAS WHERE CODIGO_DISCIPLINA = '");
  strcat(querry_command, search_code);
  strcat(querry_command, "';");

	execute_querry(con, querry_command);

	gettimeofday(&end_time, NULL);
  send_results(con, sockfd);

	return end_time;
}

// Dado o código de uma disciplina, retornar todas as informações desta disciplina
TIME get_full_info(MYSQL *con, int sockfd, char *search_code)
{
  char querry_command[1000];
	TIME end_time;

	strcpy(querry_command, "SELECT * FROM DISCIPLINAS WHERE CODIGO_DISCIPLINA = '");
	strcat(querry_command, search_code);
	strcat(querry_command, "';");

	execute_querry(con, querry_command);

	gettimeofday(&end_time, NULL);
  send_results(con, sockfd);

	return end_time;
}

// Listar todas as informações de todas as disciplinas
TIME get_all_info(MYSQL *con, int sockfd)
{
  char querry_command[1000];
	TIME end_time;

  strcpy(querry_command, "SELECT * FROM DISCIPLINAS;");
  execute_querry(con, querry_command);

	gettimeofday(&end_time, NULL);
  send_results(con, sockfd);

	return end_time;
}

// *********** Operacoes do PROFESSOR *********** //

// Escrever um texto de comentário sobre a próxima aula de uma disciplina (apenas usuário professor)
TIME write_comment(MYSQL *con, int sockfd, char *search_code, char *comment)
{
  char querry_command[1000], response[6];
	TIME end_time;

  strcpy(querry_command, "UPDATE DISCIPLINAS SET COMENTARIO = '");
  strcat(querry_command, comment);
  strcat(querry_command, "' WHERE CODIGO_DISCIPLINA = '");
  strcat(querry_command, search_code);
  strcat(querry_command, "';");

	execute_querry(con, querry_command);

	sprintf(response, "%d", (int)mysql_affected_rows(con));

	gettimeofday(&end_time, NULL);
	write_buffer(sockfd, response);

	return end_time;
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

void function_time_eval(TIME before, TIME after, char opcode[5])
{
	TIME diff;
	char filename[50] = "time_log/server_operation_";

	strcat(filename, opcode);
	strcat(filename, ".txt");

	FILE *f = fopen(filename, "a");
	if (f == NULL)
	{
    	printf("Error opening file!\n");
    	exit(1);
	}

	if(!timeval_subtract(&diff, &after, &before))
	{
			fprintf(f, "%ld.%06ld\n", diff.tv_sec, diff.tv_usec);
	}
	fclose(f);
}
