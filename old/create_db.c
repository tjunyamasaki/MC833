// For more information, check http://zetcode.com/db/mysqlc/
#include <stdio.h>
#include <string.h>
#include <my_global.h>
#include <mysql.h>

void finish_with_error(MYSQL *con);
void execute_querry(MYSQL *con, char* querry_command);
void start_db(MYSQL *con);

int main(int argc, char **argv)
{
  MYSQL *con = mysql_init(NULL);

  if (con == NULL)
  {
      fprintf(stderr, "%s\n", mysql_error(con));
      exit(1);
  }

  char username[25], password[25];

  printf("Enter username:\n");
  scanf("%s", username);
  printf("Enter password:\n");
  scanf("%s", password);

  if (mysql_real_connect(con, "localhost", username, password, NULL, 0, NULL, 0) == NULL)
  {
      finish_with_error(con);
  }

  start_db(con);

  mysql_close(con);
  exit(0);
}

void finish_with_error(MYSQL *con)
{
  fprintf(stderr, "%s\n", mysql_error(con));
  mysql_close(con);
  exit(1);
}

void execute_querry(MYSQL *con, char* querry_command)
{
  if (mysql_query(con, querry_command)) {
      finish_with_error(con);
  }
}

void start_db(MYSQL *con)
{
  char querry_command[1000];

  strcpy(querry_command, "CREATE SCHEMA IF NOT EXISTS projeto1 DEFAULT CHARACTER SET utf8;");
  execute_querry(con, querry_command);

  strcpy(querry_command, "USE projeto1;");
  execute_querry(con, querry_command);

  strcpy(querry_command, "DROP TABLE IF EXISTS DISCIPLINAS;");
  execute_querry(con, querry_command);

  strcpy(querry_command, "CREATE TABLE DISCIPLINAS(CODIGO_DISCIPLINA VARCHAR(5) NOT NULL, TITULO VARCHAR(250) NULL, EMENTA VARCHAR(1000) NULL, SALA VARCHAR(5) NULL, HORARIO VARCHAR(45) NULL, COMENTARIO VARCHAR(1500) NULL, PRIMARY KEY (CODIGO_DISCIPLINA))");
  execute_querry(con, querry_command);


  strcpy(querry_command, "DROP TABLE IF EXISTS USUARIOS;");
  execute_querry(con, querry_command);

  strcpy(querry_command, "CREATE TABLE USUARIOS(USERNAME VARCHAR(45) NOT NULL, TIPO VARCHAR(45) NULL, PRIMARY KEY (USERNAME))");
  execute_querry(con, querry_command);

  // strcpy(querry_command, "LOAD DATA INFILE 'DATA_DIR/data/DISCIPLINAS.csv' IGNORE INTO TABLE DISCIPLINAS CHARACTER SET utf8 FIELDS TERMINATED BY ',' OPTIONALLY ENCLOSED BY '\"' LINES TERMINATED BY '\r\n' IGNORE 1 LINES (CODIGO_DISCIPLINA,TITULO,EMENTA,SALA,HORARIO,COMENTARIO);");
  // execute_querry(con, querry_command);

  strcpy(querry_command, "INSERT INTO USUARIOS (USERNAME,TIPO) VALUES('root','admin');");
  execute_querry(con, querry_command);

  // Creating dummy professor for test
  strcpy(querry_command, "INSERT INTO USUARIOS (USERNAME,TIPO) VALUES('professor','professor');");
  execute_querry(con, querry_command);

  strcpy(querry_command, "GRANT SELECT, UPDATE ON projeto1.* TO 'professor'@'localhost' IDENTIFIED BY 'senha123';");
  execute_querry(con, querry_command);

  strcpy(querry_command, "FLUSH PRIVILEGES;");
  execute_querry(con, querry_command);
  // Creating dummy student for test
  strcpy(querry_command, "INSERT INTO USUARIOS (USERNAME,TIPO) VALUES('aluno','aluno');");
  execute_querry(con, querry_command);

  strcpy(querry_command, "GRANT SELECT ON projeto1.* TO 'aluno'@'localhost' IDENTIFIED BY 'senha123';");
  execute_querry(con, querry_command);

  strcpy(querry_command, "FLUSH PRIVILEGES;");
  execute_querry(con, querry_command);

}
