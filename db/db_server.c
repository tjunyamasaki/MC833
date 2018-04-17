// For more information, check http://zetcode.com/db/mysqlc/

//Includes std e sql
#include <stdio.h>
#include <string.h>
#include <my_global.h>
#include <mysql.h>

//[SERVER] Includes socket/server
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


//[SERVER] CONSTANTS
#define PORT "3490"  // the port users will be connecting to
#define BACKLOG 10	 // how many pending connections queue will hold
#define BUFFER_SIZE 1024


//[SERVER] Funcoes servidor
void sigchld_handler(int s);
void *get_in_addr(struct sockaddr *sa);

void read_buffer(int sockfd, char *buffer, int bufferlen);
void read_buffer(int sockfd, char *buffer, int bufferlen);


// Funcoes auxiliares
void finish_with_error(MYSQL *con);
void login_server(MYSQL *con, int new_fd, char *buffer);
void execute_querry(MYSQL *con, char* querry_command);
void display_results(MYSQL *con);
void get_user_role(MYSQL *con, char *user_role, char *username);

// Funcoes dos administradores do BD
void list_users(MYSQL *con);
void create_user(MYSQL *con);
void delete_user(MYSQL *con);

// Funcoes dos alunos
void list_codes(MYSQL *con);
void get_ementa(MYSQL *con);
void get_comment(MYSQL *con);
void get_full_info(MYSQL *con);
void get_all_info(MYSQL *con);

// Funcoes dos professores
void write_comment(MYSQL *con);

int main(int argc, char **argv)
{

  //[SERVER]
  int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
  struct addrinfo hints, *servinfo, *p;
  struct sockaddr_storage their_addr; // connector's address information
  socklen_t sin_size;
  struct sigaction sa;
  int yes=1;
  char s[INET6_ADDRSTRLEN];
  int rv;

  // Buffer
  char buffer[BUFFER_SIZE];

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
      perror("Server: socket");
      continue;
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
        sizeof(int)) == -1) {
      perror("setsockopt");
      exit(1);
    }

    if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      close(sockfd);
      perror("Server: bind");
      continue;
    }

    break;
  }

  freeaddrinfo(servinfo); // all done with this structure

  if (p == NULL)  {
    fprintf(stderr, "Server: failed to bind\n");
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

  printf("Server: Waiting for connections...\n");




  while(1)
  {

    sin_size = sizeof their_addr;
    new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
    if (new_fd == -1) {
      perror("accept");
      continue;
    }

    inet_ntop(their_addr.ss_family,
      get_in_addr((struct sockaddr *)&their_addr),
      s, sizeof s);
    printf("Got connection from %s\n\n", s);

    while(1) {
      // CONEXAO COM O BANCO?
      MYSQL *con = mysql_init(NULL);

      if (con == NULL)
      {
        fprintf(stderr, "%s\n", mysql_error(con));
        exit(1);
      }

      int menu_code;

      // printf("Choose one of the operations below:\n");
      //
      // printf("1. Login\n");
      // printf("0. Exit\n");

      print_wait("Menu_code");

      bzero(buffer,BUFFER_SIZE);

      read_buffer(new_fd, buffer, BUFFER_SIZE-1);

      menu_code = atoi(buffer);

      switch (menu_code)
      {
        case 1:
        printf("-- Operação login --\n\n");
        login_server(con, new_fd, buffer);
        break;

        case 0:
        printf("Fechando conexao!\n");
        exit(0);
        break;

        default:
        printf("Operacao invalida. Selecione outra.\n");
        break;
      }

      close(sockfd);
    }
  }
  exit(0);
}

// ********************** Funções auxiliares *************************//

void print_wait(char *msg){
  printf("--> Waiting %s...\n", msg);
}


// *******************[SERVER] Funcoes servidor ********************** //

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



// *********************** Funcoes AUXILIARES *********************** //

void login_server(MYSQL *con, int new_fd, char *buffer)
{
  char username[25], password[25];

  // printf("Enter username: ");
  // scanf("%s", username);
  // printf("Enter password: ");
  // scanf("%s", password);

  //LENDO USER AND PASSWORD
  print_wait("Username");
  read_buffer(new_fd, buffer, BUFFER_SIZE-1);
  strcpy(username, buffer);

  //LENDO NADA 1
  read_buffer(new_fd, buffer, BUFFER_SIZE-1);


  print_wait("Passwaord");
  read_buffer(new_fd, buffer, BUFFER_SIZE-1);
  strcpy(password, buffer);

  printf("username: %s\n", username);
  printf("password: %s\n", password);

  if (mysql_real_connect(con, "localhost",
      username, password, "projeto1", 0, NULL, 0) == NULL)
  {
      finish_with_error(con);
  }

  // printf("morreu?\n");

  // Descobre se username eh admin, professor ou aluno.
  char user_role[20];
  get_user_role(con, user_role, username);

  write_buffer(new_fd, user_role, 20);
  // printf("morreu?\n");

  printf("\n--------------------------------------- \n");
  printf("Bem vindo %s '%s'!\n", user_role, username);
  printf("--------------------------------------- \n\n");


  //LENDO NADA 2
  read_buffer(new_fd, buffer, 2);

  int login=1;
  while(login == 1)
  {
    int op_code;

    print_wait("Opcode");

    bzero(buffer, BUFFER_SIZE);
    read_buffer(new_fd, buffer, 2);

    op_code = atoi(buffer);

    printf("op code: %d\n", op_code);
    // Usuario eh Admin do BD.
    if(strcmp(user_role, "admin") == 0)
    {
      // scanf("%d", &op_code);

      switch (op_code)
      {
        case -2:
          printf("\nEncerrando servidor!\n");
          mysql_close(con);
          exit(0);
          break;
        case -1:
          printf("\nFinalizando sessao!\n");
          login = 0;
          mysql_close(con);
          break;
        case 1:
          list_users(con);
          break;
        case 2:
          create_user(con);
          break;
        case 3:
          delete_user(con);
          break;
        default:
          printf("\nOperacao invalida. Selecione outra.\n");
          break;
      }
    }
    // Usuario eh Aluno ou Professor.
    else
    {
      // scanf("%d", &op_code);

      // bzero(buffer, BUFFER_SIZE);
      // read_buffer(new_fd, buffer, 2);
      //
      // op_code = atoi(buffer);

      // printf("op code: %d\n", op_code);

      switch (op_code)
      {
        case -2:
          printf("\nEncerrando servidor!\n");
          mysql_close(con);
          exit(0);
          break;
        case -1:
          printf("\nFinalizando sessao!\n");
          login = 0;
          mysql_close(con);
          break;
        case 1:
          list_codes(con);
          break;
        case 2:
          get_ementa(con);
          break;
        case 3:
          get_comment(con);
          break;
        case 4:
          get_full_info(con);
          break;
        case 5:
          get_all_info(con);
          break;
        case 6:
          if(strcmp(user_role, "professor") == 0)
          {
            write_comment(con);
          }
          else
          {
            printf("\nOperacao invalida, aluno espertinho! Tente novamente.\n");
          }
          break;
        default:
          printf("\nOperacao invalida. Selecione outra.\n");
          break;
      }
    }
  }

  printf("-- Login server end --\n\n");
}

// Finaliza conexao apresentando erros
void finish_with_error(MYSQL *con)
{
  fprintf(stderr, "%s\n", mysql_error(con));
  mysql_close(con);
  exit(1);
}

// Executa querry
void execute_querry(MYSQL *con, char* querry_command)
{
  if (mysql_query(con, querry_command)) {
      finish_with_error(con);
  }
}

// Printa resultados de uma consulta
void display_results(MYSQL *con)
{
  MYSQL_RES *result = mysql_store_result(con);

  if (result == NULL)
  {
    finish_with_error(con);
  }
  int num_fields = mysql_num_fields(result);
  int num_rows = mysql_num_rows(result);

  MYSQL_ROW row;
  MYSQL_FIELD *field;

  // Lembrar da ideia de salvar tudo em uma matriz de char*, achar o maxlenght de cada coluna, e rodar um for printando espacos pra igualar a diferenca de tamanho entre cada item da coluna.

  printf("--------------------------------------- \n");
  while(field = mysql_fetch_field(result))
  {
     printf("%s  ", field->name);
  }
  printf("\n---------------------------------------\n");

  while ((row = mysql_fetch_row(result)))
  {
      for(int i = 0; i < num_fields; i++)
      {
          printf("%s  ", row[i] ? row[i] : "NULL");
      }
      printf("\n");
  }
  printf("---------------------------------------\n\n");
  mysql_free_result(result);
}

void get_user_role(MYSQL *con, char *user_role, char *username)
{
  char querry_command[1000];

  strcpy(querry_command, "SELECT TIPO FROM USUARIOS WHERE USERNAME = '");
  strcat(querry_command, username);
  strcat(querry_command, "';");
  execute_querry(con, querry_command);

  MYSQL_RES *result = mysql_store_result(con);

  if (result == NULL)
  {
      finish_with_error(con);
  }

  int num_fields = mysql_num_fields(result);

  MYSQL_ROW row;

  while ((row = mysql_fetch_row(result)))
  {
      for(int i = 0; i < num_fields; i++)
      {
          strcpy(user_role, row[i]);
      }
  }
  mysql_free_result(result);
}

// *********************** Operacoes do ADMIN *********************** //

// Lista usuarios cadastrados no BD
void list_users(MYSQL *con)
{
  char querry_command[250] = "SELECT User FROM mysql.user;";

  printf("\n---------------------------------------\n");
  printf(" 1 -> Listar usuarios\n");
  printf("---------------------------------------\n\n");

  execute_querry(con, querry_command);
  display_results(con);
}

// Cria usuario no BD
void create_user(MYSQL *con)
{
  char username[25], password[25], querry_command[1000], insert_command[1000];
  int user_type;

  printf("\n---------------------------------------\n");
  printf(" 2 -> Criar usuario\n");
  printf("---------------------------------------\n\n");

  printf("Digite o username que sera criado:\n");
  scanf("%s", username);

  printf("\nDigite a senha desejada:\n");
  scanf("%s", password);

  printf("\nTipos de usuario disponiveis:\n");
  printf("1 -> Aluno:\n");
  printf("2 -> Professor:\n");

  printf("Selecione o tipo:\n");
  scanf("%d", &user_type);

  strcpy(insert_command, "INSERT INTO USUARIOS (USERNAME,TIPO) VALUES('");
  strcat(insert_command, username);

  switch (user_type) {
    case 1:
      strcpy(querry_command, "GRANT SELECT ON projeto1.* TO '");
      strcat(insert_command,"','aluno');");
      break;
    case 2:
      strcpy(querry_command, "GRANT SELECT, UPDATE ON projeto1.* TO '");
      strcat(insert_command,"','professor');");
      break;
    default:
      printf("Tipo invalido. Abortando operacao.\n");
      return;
  }
  strcat(querry_command, username);
  strcat(querry_command, "'@'localhost' IDENTIFIED BY '");
  strcat(querry_command, password);
  strcat(querry_command, "';");
  execute_querry(con, querry_command);

  strcpy(querry_command, "FLUSH PRIVILEGES;");
  execute_querry(con, querry_command);
  execute_querry(con, insert_command);

  printf("--------------------------------------- \n\n");
}

// Deleta algum usuario do BD
void delete_user(MYSQL *con)
{
  char username[25], querry_command[1000];

  printf("\n---------------------------------------\n");
  printf(" 3 -> Deletar usuario\n");
  printf("---------------------------------------\n\n");

  printf("Digite o username que sera deletado:\n");
  scanf("%s", username);

  strcpy(querry_command, "DROP USER '");
  strcat(querry_command, username);
  strcat(querry_command, "'@'localhost';");
  execute_querry(con, querry_command);

  strcpy(querry_command, "DELETE FROM USUARIOS WHERE USERNAME='");
  strcat(querry_command, username);
  strcat(querry_command, "';");
  execute_querry(con, querry_command);

  strcpy(querry_command, "FLUSH PRIVILEGES;");
  execute_querry(con, querry_command);

  printf("--------------------------------------- \n\n");
}

// *********************** Operacoes ALUNO/PROFESSOR *********************** //

// Listar todos os códigos de disciplinas com seus respectivos títulos;
void list_codes(MYSQL *con)
{
  char querry_command[1000];

  printf("\n---------------------------------------\n");
  printf(" 1 -> Listar codigos das disciplinas\n");
  printf("---------------------------------------\n\n");

  strcpy(querry_command, "SELECT CODIGO_DISCIPLINA AS CODIGO, TITULO FROM DISCIPLINAS;");
  execute_querry(con, querry_command);

  display_results(con);
}

// Dado o código de uma disciplina, retornar a ementa;
void get_ementa(MYSQL *con)
{
  char search_code[6], querry_command[1000];

  printf("\n---------------------------------------\n");
  printf(" 2 -> Buscar ementa\n");
  printf("---------------------------------------\n\n");

  printf("Digite o codigo da disciplina desejada:\n");
  scanf("%s", search_code);

  strcpy(querry_command, "SELECT EMENTA FROM DISCIPLINAS WHERE CODIGO_DISCIPLINA = '");
  strcat(querry_command, search_code);
  strcat(querry_command, "';");
  execute_querry(con, querry_command);

  display_results(con);
}

// Dado o código de uma disciplina, retornar o texto de comentário sobre a próxima aula.
void get_comment(MYSQL *con)
{
  char search_code[6], querry_command[1000];

  printf("\n---------------------------------------\n");
  printf(" 3 -> Buscar comentario sobre a proxima aula\n");
  printf("---------------------------------------\n\n");

  printf("Digite o codigo da disciplina desejada:\n");
  scanf("%s", search_code);

  strcpy(querry_command, "SELECT COMENTARIO FROM DISCIPLINAS WHERE CODIGO_DISCIPLINA = '");
  strcat(querry_command, search_code);
  strcat(querry_command, "';");
  execute_querry(con, querry_command);

  display_results(con);
}

// Dado o código de uma disciplina, retornar todas as informações desta disciplina;
void get_full_info(MYSQL *con)
{
  char search_code[6], querry_command[1000];

  printf("\n---------------------------------------\n");
  printf(" 4 -> Listar informacoes de uma disciplina\n");
  printf("---------------------------------------\n\n");

  printf("Digite o codigo da disciplina desejada:\n");
  scanf("%s", search_code);

  strcpy(querry_command, "SELECT * FROM DISCIPLINAS WHERE CODIGO_DISCIPLINA = '");
  strcat(querry_command, search_code);
  strcat(querry_command, "';");
  execute_querry(con, querry_command);

  display_results(con);
}

// Listar todas as informações de todas as disciplinas
void get_all_info(MYSQL *con)
{
  char querry_command[1000];

  printf("\n---------------------------------------\n");
  printf(" 5 -> Listar informacoes de todas as disciplinas\n");
  printf("---------------------------------------\n\n");

  strcpy(querry_command, "SELECT * FROM DISCIPLINAS;");
  execute_querry(con, querry_command);

  display_results(con);
}

// *********************** Operacoes do PROFESSOR *********************** //

// Escrever um texto de comentário sobre a próxima aula de uma disciplina (apenas usuário professor)
void write_comment(MYSQL *con)
{
  char search_code[6], comment[500], querry_command[1000];

  printf("\n---------------------------------------\n");
  printf(" 6 -> Escrever comentario sobre a proxima aula de uma disciplina\n");
  printf("---------------------------------------\n\n");

  printf("Digite o codigo da disciplina desejada:\n");
  scanf("%s", search_code);

  printf("\nDigite o comentario que deseja inserir em %s:\n", search_code);
  scanf(" ", search_code);
  fgets(comment, sizeof(comment), stdin);

  strcpy(querry_command, "UPDATE DISCIPLINAS SET COMENTARIO = '");
  strcat(querry_command, comment);
  strcat(querry_command, "' WHERE CODIGO_DISCIPLINA = '");
  strcat(querry_command, search_code);
  strcat(querry_command, "';");

  execute_querry(con, querry_command);
  printf("---------------------------------------\n\n");
}
