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
void write_buffer(int sockfd, char *msg, int msglen);
void read_buffer(int sockfd, char *buffer, int bufferlen);

// ****************** [MYSQL] ************************** //

void finish_with_error(MYSQL *con);
void execute_querry(MYSQL *con, char* querry_command);
void loginMysql(char *username, char *password, MYSQL *con);
void initError(MYSQL *con);

// ******************* Project related functions ******************** //

void professor(int sockfd, int opcde);
void aluno(int sockfd, int opcde);


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

				switch (user) {
					case 1:
						printa("PROFESSOR Logged In!");
						loginMysql("professor", "senha123", con);
						// int num = 1;
						while(opcode){
							printa("\nWaiting for opcode..\n");
							read_buffer(new_fd, buf, 12);
							opcode = atoi(buf);

							//IMPLEMENTAR PROFESSOR
							//UMA FUNCAO PRA CADA OPCODE
							//SEND OPCODE CONTENT
							professor(new_fd, opcode);

							//temp APAGAR feed back do op code
							write_buffer(new_fd, "Got Op code!", 12);
							//temp APAGAR
						}
					break;
					case 2:
						printa("ALUNO Logged In!");
						loginMysql("aluno", "senha123", con);
						while(opcode){
							printa("\nWaiting for opcode..\n");
							read_buffer(new_fd, buf, 12);
							opcode = atoi(buf);

							//IMPLEMENTAR PROFESSOR
							//UMA FUNCAO PRA CADA OPCODE
							//SEND OPCODE CONTENT
							aluno(new_fd, opcode);

							//temp APAGAR feed back do op code
							write_buffer(new_fd, "Got Op code!", 12);
							//temp APAGAR
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
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

// ***************** WRITE AND READ SOCKET *******************//
void write_buffer(int sockfd, char *msg, int msglen) {

	printf("---> Sending this msg: %s\n\n", msg);

	int num = write(sockfd, msg, msglen);

	if (num < 0) {
		perror("ERROR: Writing to socket didnt go well..");
		exit(0);
	}
}

void read_buffer(int sockfd, char *buffer, int bufferlen) {

  printf("---> Reading..\n");

	int num = read(sockfd, buffer, bufferlen);

  printf("---> What was read: %s\n\n", buffer);

	if (num < 0) {
		perror("ERROR: Reading from socket didnt go well..");
		exit(0);
	}
}

// ****************** [MYSQL] ************************** //

void initError(MYSQL *con) {
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

// Executa querry
void execute_querry(MYSQL *con, char* querry_command)
{
  if (mysql_query(con, querry_command)) {
      finish_with_error(con);
  }
}

void loginMysql(char *username, char *password, MYSQL *con){
	if (mysql_real_connect(con, "localhost",
	username, password, "projeto1", 0, NULL, 0) == NULL)
	{
		finish_with_error(con);
	}
}


// ******************* Project related functions ******************** //

void professor(int sockfd, int opcde) {

}

void aluno(int sockfd, int opcde) {

}
