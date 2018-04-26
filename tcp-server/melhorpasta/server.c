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


#define PORT "8000"  // the port users will be connecting to
#define BACKLOG 10	 // how many pending connections queue will hold
#define MAXDATASIZE 100 // max number of bytes we can get at once

// ****************** Prints ***************************** //

void printa(char *msg);

// ************** [Server] - Basic functions ********************** //

void sigchld_handler(int s);
// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa);
void write_buffer(int sockfd, char *msg);
void read_buffer(int sockfd, char *msg, int len);

// ****************** [MYSQL] ************************** //

void initError(MYSQL *con);
void finish_with_error(MYSQL *con);
void loginMysql(char *username, char *password, MYSQL *con);
void execute_querry(MYSQL *con, char* querry_command);
void display_results(MYSQL *con, int sockfd);

// ******************* Project related functions ******************** //

// Operacoes dos alunos/professores
void list_codes(MYSQL *con, int sockfd);
void get_ementa(MYSQL *con, int sockfd);
void get_comment(MYSQL *con, int sockfd);
void get_full_info(MYSQL *con, int sockfd);
void get_all_info(MYSQL *con, int sockfd);

// Operacoes dos professores
void write_comment(MYSQL *con, int sockfd);

void professor(MYSQL *con, int sockfd, int opcde);
void aluno(MYSQL *con, int sockfd, int opcde);


int main(void)
{
	int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	struct sigaction sa;
	int yes=1;
	char s[INET6_ADDRSTRLEN];
	int rv;

	char buf[MAXDATASIZE];

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
		}

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
				sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1);
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("server: bind");
			continue;
		}

		break;
	}

	freeaddrinfo(servinfo); // all done with this structure

	if (p == NULL)  {
		fprintf(stderr, "server: failed to bind\n");
		exit(1);
	}

	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}

	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}


	while(1) {

		printa("Waiting for new connections..");

		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		if (new_fd == -1) {
			perror("accept");
			continue;
		}

		inet_ntop(their_addr.ss_family,
			get_in_addr((struct sockaddr *)&their_addr),
			s, sizeof s);
		printf("server: got connection from %s\n", s);

		if (!fork()) {

			close(sockfd); // child doesn't need the listener

			// -------------------------------------------------------------- //
			// ------------------------ MAIN SERVER CODE -------------------- //
			// -------------------------------------------------------------- //

			int user = 1;
			int opcode;

			while(user){

				MYSQL *con = mysql_init(NULL);
				initError(con);

				//get login from client (professor ou aluno)
				printa("\nWaiting for login type..\n");
				read_buffer(new_fd, buf, 1);
				user = atoi(buf);
				opcode = 1;

				switch(user){
					case 1:
						printa("PROFESSOR Logged In!");
						loginMysql("professor", "senha123", con);
						// int num = 1;
						while(opcode){
							printa("\nWaiting for opcode..\n");
							read_buffer(new_fd, buf, 1);
							opcode = atoi(buf);

							professor(con, new_fd, opcode);
						}
						break;
					case 2:
						printa("ALUNO Logged In!");
						loginMysql("aluno", "senha123", con);
						while(opcode){
							printa("\nWaiting for opcode..\n");
							read_buffer(new_fd, buf, 1);
							opcode = atoi(buf);

							aluno(con, new_fd, opcode);
						}
						break;
					case 0:
						printa("Closing Connection..\n");
						break;
					default:
						printa("Switch case unexpected case..");

				}

				//Closing sql connection
				mysql_close(con);
			}

			// -------------------------------------------------------------- //
			// ------------------------ MAIN SERVER CODE -------------------- //
			// -------------------------------------------------------------- //
			close(new_fd);
			printa("Socket new_fd closed..");
			exit(0);
		}
		close(new_fd);  // parent doesn't need this
	}

	return 0;
}

// ****************** FUNCTIONS *************************** //

// ****************** Prints ******************************//

void printa(char *msg){
	printf("%s\n", msg);
}

// ************** [Server] - Basic functions ********************** //

void sigchld_handler(int s)
{
	(void)s; // quiet unused variable warning

	// waitpid() might overwrite errno, so we save and restore it:
	int saved_errno = errno;

	while(waitpid(-1, NULL, WNOHANG) > 0);
	errno = saved_errno;
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET){
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

// ***************** WRITE AND READ SOCKET ******************* //

void write_buffer(int sockfd, char *msg) {

	// printf("---> Sending this msg: %s\n\n", msg);
	//
	// int num = write(sockfd, msg, msglen);
	//
	// if (num < 0) {
	// 	perror("ERROR: Writing to socket didnt go well..");
	// 	exit(0);
	// }



	int tam = strlen(msg);
	char tamemstring[10];

	printf("tamanho da frase: %d\n", tam);

	sprintf(tamemstring, "%d", tam);

	int num = send(sockfd, tamemstring, sizeof(int), 0);
	if (num < 0) {
		perror("ERROR: Writing to socket didnt go well..");
		exit(0);
	}

	for (int i = 0; i < tam; i = i+MAXDATASIZE) {
		printf("sending: %s\n", msg+i);
		num = send(sockfd, msg+i, MAXDATASIZE, 0);
		if (num < 0) {
			perror("ERROR: Writing to socket didnt go well..");
			exit(0);
		}
	}
}

void read_buffer(int sockfd, char *buffer, int bufferlen) {

  printf("---> Reading..\n");

	int num = recv(sockfd, buffer, bufferlen, 0);

  printf("---> What was read: %s\n\n", buffer);

	if (num < 0) {
		perror("ERROR: Reading from socket didnt go well..");
		exit(0);
	}
}

// ****************** [MYSQL] ************************** //

void initError(MYSQL *con){
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

void loginMysql(char *username, char *password, MYSQL *con){
	if (mysql_real_connect(con, "localhost",
	username, password, "projeto1", 0, NULL, 0) == NULL)
	{
		finish_with_error(con);
	}
}

// Executa querry
void execute_querry(MYSQL *con, char* querry_command)
{
  if (mysql_query(con, querry_command)){
      finish_with_error(con);
  }
}

// Printa resultados de uma consulta
void display_results(MYSQL *con, int sockfd)
{
  MYSQL_RES *result = mysql_store_result(con);

  if (result == NULL)
  {
    finish_with_error(con);
  }
  int num_fields = mysql_num_fields(result);
  int num_rows = mysql_num_rows(result);
	char table[2500];

	strcpy(table, "");

	MYSQL_ROW row;
	MYSQL_FIELD *field;

	strcat(table,"--------------------------------------- \n");
	while(field = mysql_fetch_field(result))
	{
		strcat(table, field->name);
		strcat(table, " ");
	}
	strcat(table,"\n---------------------------------------\n");

	while ((row = mysql_fetch_row(result)))
	{
			for(int i = 0; i < num_fields; i++)
			{
					//printf("%s  ", row[i] ? row[i] : "NULL");

					strcat(table, row[i]);
					strcat(table, " ");
			}
			strcat(table,"\n");
	}
	strcat(table,"---------------------------------------\n\n");

	printf("\n\n\n%s\n\n\n", table);


	write_buffer(sockfd, table);
	mysql_free_result(result);

}

// *********************** Operacoes ALUNO/PROFESSOR *********************** //

// Listar todos os códigos de disciplinas com seus respectivos títulos;
void list_codes(MYSQL *con, int sockfd)
{
  char querry_command[1000];

  strcpy(querry_command, "SELECT CODIGO_DISCIPLINA AS CODIGO, TITULO FROM DISCIPLINAS;");
  execute_querry(con, querry_command);

	// Alterar para send_results
  display_results(con, sockfd);
}

// Dado o código de uma disciplina, retornar a ementa;
void get_ementa(MYSQL *con, int sockfd)
{
	int size;
  char search_code[6], querry_command[1000];

  // Adicionar receive_parameter
	recv(sockfd, search_code, 6, 0);
  // scanf("%s", search_code);

  strcpy(querry_command, "SELECT EMENTA FROM DISCIPLINAS WHERE CODIGO_DISCIPLINA = '");
  strcat(querry_command, search_code);
  strcat(querry_command, "';");
  execute_querry(con, querry_command);

  // Alterar para send_results
	display_results(con, sockfd);
}

// Dado o código de uma disciplina, retornar o texto de comentário sobre a próxima aula.
void get_comment(MYSQL *con, int sockfd)
{
	int size;
  char search_code[6], querry_command[1000];

	// Adicionar receive_parameter
	recv(sockfd, search_code, 6, 0);
  // scanf("%s", search_code);

  strcpy(querry_command, "SELECT COMENTARIO FROM DISCIPLINAS WHERE CODIGO_DISCIPLINA = '");
  strcat(querry_command, search_code);
  strcat(querry_command, "';");
  execute_querry(con, querry_command);

	// Alterar para send_results
  display_results(con, sockfd);
}

// Dado o código de uma disciplina, retornar todas as informações desta disciplina;
void get_full_info(MYSQL *con, int sockfd)
{
	int size;
  char search_code[6], querry_command[1000];

	// Adicionar receive_parameter
	recv(sockfd, search_code, 6, 0);
  //scanf("%s", search_code);

  strcpy(querry_command, "SELECT * FROM DISCIPLINAS WHERE CODIGO_DISCIPLINA = '");
  strcat(querry_command, search_code);
  strcat(querry_command, "';");
  execute_querry(con, querry_command);

	// Alterar para send_results
  display_results(con, sockfd);
}

// Listar todas as informações de todas as disciplinas
void get_all_info(MYSQL *con, int sockfd)
{
  char querry_command[1000];

  strcpy(querry_command, "SELECT * FROM DISCIPLINAS;");
  execute_querry(con, querry_command);

	// Alterar para send_results
  display_results(con, sockfd);
}

// *********************** Operacoes do PROFESSOR *********************** //

// Escrever um texto de comentário sobre a próxima aula de uma disciplina (apenas usuário professor)
void write_comment(MYSQL *con, int sockfd)
{
	int size;
  char search_code[6], comment[500], querry_command[1000];

  // Adicionar receive_parameter
	recv(sockfd, search_code, 6, 0);
  //scanf("%s", search_code);

  // Adicionar receive_parameter
	// Adicionar print_do_receive
	recv(sockfd, comment, 500, 0);
  //fgets(comment, sizeof(comment), stdin);

  strcpy(querry_command, "UPDATE DISCIPLINAS SET COMENTARIO = '");
  strcat(querry_command, comment);
  strcat(querry_command, "' WHERE CODIGO_DISCIPLINA = '");
  strcat(querry_command, search_code);
  strcat(querry_command, "';");

  execute_querry(con, querry_command);
}

// ******************* Project related functions ******************** //

void professor(MYSQL *con, int sockfd, int opcode) {
	switch (opcode)
	{
		case 0:
			printf("\nFinalizando sessao!\n");
			break;
		case 1:
			list_codes(con, sockfd);
			break;
		case 2:
			get_ementa(con, sockfd);
			break;
		case 3:
			get_comment(con, sockfd);
			break;
		case 4:
			get_full_info(con, sockfd);
			break;
		case 5:
			get_all_info(con, sockfd);
			break;
		case 6:
			write_comment(con, sockfd);
			break;
		default:
			printf("\nOperacao invalida. Selecione outra.\n");
			break;
	}
}

void aluno(MYSQL *con, int sockfd, int opcode) {
	switch (opcode)
	{
		case 0:
			printf("\nFinalizando sessao!\n");
			break;
		case 1:
			list_codes(con, sockfd);
			break;
		case 2:
			get_ementa(con, sockfd);
			break;
		case 3:
			get_comment(con, sockfd);
			break;
		case 4:
			get_full_info(con, sockfd);
			break;
		case 5:
			get_all_info(con, sockfd);
			break;
		default:
			printf("\nOperacao invalida. Selecione outra.\n");
			break;
	}
}
