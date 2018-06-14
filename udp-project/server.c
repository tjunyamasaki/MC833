#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//[MYSQL]
#include <my_global.h>
#include <mysql.h>

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

#define PORT 8000  // Porta a qual o cliente se conecta
#define MAXBUFLEN 513 // Numero maximo de bytes que sao recebidos em um pacote

// ************** [Client/Server] - Basic functions ************** //

void sigchld_handler(int s); // Get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa);
void write_buffer(int sockfd, ADDRESS *their_addr, char *msg);
int read_buffer(int sockfd, ADDRESS *their_addr, Message *msg);


// ****************** [MYSQL] ************************** //

void initError(MYSQL *con);
void finish_with_error(MYSQL *con);
void loginMysql(char *username, char *password, MYSQL *con);
void execute_querry(MYSQL *con, char* querry_command);
void send_results(MYSQL *con, int sockfd, ADDRESS *their_addr);

// ******************* Funcoes especificas do projeto ******************** //

void professor(int sockfd, ADDRESS *their_addr, TIME start_time, Message *msg);
void aluno(int sockfd, ADDRESS *their_addr, TIME start_time, Message *msg);

// Operacoes dos alunos/professores
TIME list_codes(MYSQL *con, int sockfd, ADDRESS *their_addr);
TIME get_ementa(MYSQL *con, int sockfd, ADDRESS *their_addr, char *search_code);
TIME get_comment(MYSQL *con, int sockfd, ADDRESS *their_addr, char *search_code);
TIME get_full_info(MYSQL *con, int sockfd, ADDRESS *their_addr, char *search_code);
TIME get_all_info(MYSQL *con, int sockfd, ADDRESS *their_addr);

// Operacoes dos professores
TIME write_comment(MYSQL *con, int sockfd, ADDRESS *their_addr, char *search_code, char *comment);

// ****************** Avaliacao do tempo para communicacao ****************** //
int timeval_subtract(TIME *result, TIME *x, TIME *y);
void function_time_eval(TIME before, TIME after, char opcode[5]);

// ****************** MAIN CODE ****************** //

int main(void)
{
	int sockfd;
	ADDRESS my_addr;    // My address information
	ADDRESS their_addr; // Connector’s address information

	if ((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) == -1)
	{
		perror("Socket");
		exit(1);
	}

	my_addr.sin_family = AF_INET;         // host byte order
	my_addr.sin_port = htons(PORT);     // short, network byte order
	my_addr.sin_addr.s_addr = INADDR_ANY; // automatically fill with my IP
	memset(&(my_addr.sin_zero), '\0', 8); // zero the rest of the struct

	if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1)
	{
			perror("Bind");
			exit(1);
	}

	// -------------------------------------------------------------- //
	// ------------------------ MAIN SERVER CODE -------------------- //
	// -------------------------------------------------------------- //

	while(1)
	{
		int user, received;
		Message *msg;
		TIME start_time;

		printf("\n-> Waiting for message...\n");

		msg = calloc(1, sizeof(Message));

		received = read_buffer(sockfd, &their_addr, msg);  // Mensagem vinda do Client.

		gettimeofday(&start_time, NULL);
		user = atoi(msg->usercode);

		if(received)
		{
			printf("\n*** Server: got message from %s ***\n", inet_ntoa(their_addr.sin_addr));

			switch(user)
			{
				case 1:
					professor(sockfd, &their_addr, start_time, msg);
					break;
				case 2:
					aluno(sockfd, &their_addr, start_time, msg);
					break;
				default:
					printf("\nUnexpected login option.\n");
			}
		}
		free(msg);
	}
	close(sockfd);

	return 0;
}

// ********************************** FUNCOES ********************************** //

// ***************** [Server/Client] - Funcoes Basicas ***************** //

// *********** WRITE AND READ SOCKET *********** //

void write_buffer(int sockfd, ADDRESS *their_addr, char *msg)
{
	int numbytes;
	socklen_t addr_len;

	addr_len = sizeof(struct sockaddr);

	numbytes = sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr *)their_addr, addr_len);

	if (numbytes == -1)
	{
		perror("\nERROR: Writing datagram didnt go well...\n");
		exit(0);
	}
}

int read_buffer(int sockfd, ADDRESS *their_addr, Message *msg)
{
	int numbytes;
	char *buf;
	socklen_t addr_len;

	buf = calloc(MAXBUFLEN, sizeof(char));

	addr_len = sizeof(struct sockaddr);

	numbytes = recvfrom(sockfd, buf, MAXBUFLEN , 0, (struct sockaddr *)their_addr, &addr_len);

	if (numbytes == -1)
	{
			perror("ERROR: Receiving datagram");
			exit(1);
	}

	strncpy(msg->usercode, buf, 1);
	msg->usercode[1] = '\0';
	strncpy(msg->opcode, buf+1, 1);
	msg->opcode[1] = '\0';
	strncpy(msg->search_code, buf+2, 5);
	msg->search_code[5] = '\0';
	strncpy(msg->comment, buf+7, 500);
	msg->comment[500] = '\0';

	free(buf);

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
void send_results(MYSQL *con, int sockfd, ADDRESS *their_addr)
{
  MYSQL_RES *result = mysql_store_result(con);

  if (result == NULL)
  {
    finish_with_error(con);
  }
  int num_fields = mysql_num_fields(result);

	char table[3000];

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

	write_buffer(sockfd, their_addr, table);

	mysql_free_result(result);
}

// ***************** Funcoes relacionadas ao projeto ***************** //

// *********** Interfaces *********** //

void professor(int sockfd, ADDRESS *their_addr, TIME start_time, Message *msg)
{
	MYSQL *con = mysql_init(NULL);
	initError(con);

	loginMysql("professor", "senha123", con);
	printf("\n *** PROFESSOR Logged In! ***\n");

	int opcode;
	TIME end_time;

	opcode = atoi(msg->opcode);
	printf("Received opcode %d. Running...\n", opcode);

	switch(opcode)
	{
		case 1:
			end_time = list_codes(con, sockfd, their_addr);
			function_time_eval(start_time, end_time, msg->opcode);
			break;
		case 2:
			end_time = get_ementa(con, sockfd, their_addr, msg->search_code);
			function_time_eval(start_time, end_time, msg->opcode);
			break;
		case 3:
			end_time = get_comment(con, sockfd, their_addr, msg->search_code);
			function_time_eval(start_time, end_time, msg->opcode);
			break;
		case 4:
			end_time = get_full_info(con, sockfd, their_addr, msg->search_code);
			function_time_eval(start_time, end_time, msg->opcode);
			break;
		case 5:
			end_time = get_all_info(con, sockfd, their_addr);
			function_time_eval(start_time, end_time, msg->opcode);
			break;
		case 6:
			end_time = write_comment(con, sockfd, their_addr, msg->search_code, msg->comment);
			function_time_eval(start_time, end_time, msg->opcode);
			break;
		default:
			printf("\nPedido invalido.\n");
	}

	mysql_close(con);  // Closing sql connection
}

void aluno(int sockfd, ADDRESS *their_addr, TIME start_time, Message *msg)
{
	MYSQL *con = mysql_init(NULL);
	initError(con);

	loginMysql("aluno", "senha123", con);
	printf("\n*** ALUNO Logged In! ***\n");

	int opcode;
	TIME end_time;

	opcode = atoi(msg->opcode);
	printf("\nReceived opcode %d. Running...\n", opcode);

	switch (opcode)
	{
		case 1:
			end_time = list_codes(con, sockfd, their_addr);
			function_time_eval(start_time, end_time, msg->opcode);
			break;
		case 2:
			end_time = get_ementa(con, sockfd, their_addr, msg->search_code);
			function_time_eval(start_time, end_time, msg->opcode);
			break;
		case 3:
			end_time = get_comment(con, sockfd, their_addr, msg->search_code);
			function_time_eval(start_time, end_time, msg->opcode);
			break;
		case 4:
			end_time = get_full_info(con, sockfd, their_addr, msg->search_code);
			function_time_eval(start_time, end_time, msg->opcode);
			break;
		case 5:
			end_time = get_all_info(con, sockfd, their_addr);
			function_time_eval(start_time, end_time, msg->opcode);
			break;
		default:
			printf("\nOperacao invalida. \n");
	}

	mysql_close(con);  // Closing sql connection
}

// *********** Operacoes de ALUNO e PROFESSOR *********** //

// Listar todos os códigos de disciplinas com seus respectivos títulos;
TIME list_codes(MYSQL *con, int sockfd, ADDRESS *their_addr)
{
  char querry_command[1000];
	TIME end_time;

  strcpy(querry_command, "SELECT CODIGO_DISCIPLINA AS CODIGO, TITULO FROM DISCIPLINAS;");

  execute_querry(con, querry_command);

	gettimeofday(&end_time, NULL);
  send_results(con, sockfd, their_addr);

	return end_time;
}

// Dado o código de uma disciplina, retornar a ementa;
TIME get_ementa(MYSQL *con, int sockfd, ADDRESS *their_addr, char *search_code)
{
  char querry_command[1000];
	TIME end_time;

	strcpy(querry_command, "SELECT EMENTA FROM DISCIPLINAS WHERE CODIGO_DISCIPLINA = '");
	strcat(querry_command, search_code);
	strcat(querry_command, "';");

	execute_querry(con, querry_command);

	gettimeofday(&end_time, NULL);
	send_results(con, sockfd, their_addr);

	return end_time;
}

// Dado o código de uma disciplina, retornar o texto de comentário sobre a próxima aula.
TIME get_comment(MYSQL *con, int sockfd, ADDRESS *their_addr, char *search_code)
{
  char querry_command[1000];
	TIME end_time;

	strcpy(querry_command, "SELECT COMENTARIO FROM DISCIPLINAS WHERE CODIGO_DISCIPLINA = '");
  strcat(querry_command, search_code);
  strcat(querry_command, "';");

	execute_querry(con, querry_command);

	gettimeofday(&end_time, NULL);
  send_results(con, sockfd, their_addr);

	return end_time;
}

// Dado o código de uma disciplina, retornar todas as informações desta disciplina
TIME get_full_info(MYSQL *con, int sockfd, ADDRESS *their_addr, char *search_code)
{
  char querry_command[1000];
	TIME end_time;

	strcpy(querry_command, "SELECT * FROM DISCIPLINAS WHERE CODIGO_DISCIPLINA = '");
	strcat(querry_command, search_code);
	strcat(querry_command, "';");

	execute_querry(con, querry_command);

	gettimeofday(&end_time, NULL);
  send_results(con, sockfd, their_addr);

	return end_time;
}

// Listar todas as informações de todas as disciplinas
TIME get_all_info(MYSQL *con, int sockfd, ADDRESS *their_addr)
{
  char querry_command[1000];
	TIME end_time;

  strcpy(querry_command, "SELECT * FROM DISCIPLINAS;");
  execute_querry(con, querry_command);

	gettimeofday(&end_time, NULL);
  send_results(con, sockfd, their_addr);

	return end_time;
}

// *********** Operacoes do PROFESSOR *********** //

// Escrever um texto de comentário sobre a próxima aula de uma disciplina (apenas usuário professor)
TIME write_comment(MYSQL *con, int sockfd, ADDRESS *their_addr, char *search_code, char *comment)
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
	write_buffer(sockfd, their_addr, response);

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
