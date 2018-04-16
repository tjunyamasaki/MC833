// For more information, check http://zetcode.com/db/mysqlc/
#include <stdio.h>
#include <string.h>
#include <my_global.h>
#include <mysql.h>

void finish_with_error(MYSQL *con);
void display_results(MYSQL *con);
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

  if (mysql_real_connect(con, "localhost", username, password, "projeto1", 0, NULL, 0) == NULL)
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

void display_results(MYSQL *con)
{
  MYSQL_RES *result = mysql_store_result(con);

  if (result == NULL)
  {
    finish_with_error(con);
  }
  int num_fields = mysql_num_fields(result);

  MYSQL_ROW row;
  MYSQL_FIELD *field;

  while ((row = mysql_fetch_row(result)))
  {
      for(int i = 0; i < num_fields; i++)
      {
          if (i == 0)
          {
             while(field = mysql_fetch_field(result))
             {
                printf("%s ", field->name);
             }
             printf("\n---------------------------------------\n");
          }
          printf("%s  ", row[i] ? row[i] : "NULL");
      }
  }
  printf("\n");
  mysql_free_result(result);
}

void start_db(MYSQL *con)
{
  char querry_command[1000];

  if (mysql_query(con, "DROP TABLE IF EXISTS DISCIPLINAS"))
   {
       finish_with_error(con);
   }

  strcpy(querry_command, "CREATE TABLE DISCIPLINAS(CODIGO_DISCIPLINA VARCHAR(5) NOT NULL, TITULO VARCHAR(250) NULL, EMENTA VARCHAR(1000) NULL, SALA VARCHAR(5) NULL, HORARIO VARCHAR(45) NULL, COMENTARIO VARCHAR(1500) NULL, PRIMARY KEY (CODIGO_DISCIPLINA))");

  if (mysql_query(con, querry_command)) {
      finish_with_error(con);
  }

  if (mysql_query(con, "DROP TABLE IF EXISTS USUARIOS"))
   {
       finish_with_error(con);
   }

  strcpy(querry_command, "CREATE TABLE USUARIOS(USERNAME VARCHAR(45) NOT NULL, TIPO VARCHAR(45) NULL, PRIMARY KEY (USERNAME))");

  if (mysql_query(con, querry_command)) {
      finish_with_error(con);
  }

  strcpy(querry_command, "LOAD DATA INFILE 'DATA_DIR/data/DISCIPLINAS.csv' IGNORE INTO TABLE DISCIPLINAS CHARACTER SET utf8 FIELDS TERMINATED BY ',' OPTIONALLY ENCLOSED BY '\"' LINES TERMINATED BY '\r\n' IGNORE 1 LINES (CODIGO_DISCIPLINA,TITULO,EMENTA,SALA,HORARIO,COMENTARIO);");

  if (mysql_query(con, querry_command)) {
      finish_with_error(con);
  }

  if (mysql_query(con, "INSERT INTO USUARIOS (USERNAME,TIPO) VALUES('root','admin');")) {
      finish_with_error(con);
  }

  // Creating dummy professor for test
  if (mysql_query(con, "INSERT INTO USUARIOS (USERNAME,TIPO) VALUES('professor','professor');")) {
      finish_with_error(con);
  }
  if (mysql_query(con, "GRANT SELECT, UPDATE ON projeto1.* TO 'professor'@'localhost' IDENTIFIED BY 'senha123';")) {
      finish_with_error(con);
  }
  if (mysql_query(con, "FLUSH PRIVILEGES;")) {
      finish_with_error(con);
  }

  // Creating dummy student for test
  if (mysql_query(con, "INSERT INTO USUARIOS (USERNAME,TIPO) VALUES('aluno','aluno');")) {
      finish_with_error(con);
  }
  if (mysql_query(con, "GRANT SELECT ON projeto1.* TO 'aluno'@'localhost' IDENTIFIED BY 'senha123';")) {
      finish_with_error(con);
  }
  if (mysql_query(con, "FLUSH PRIVILEGES;")) {
    finish_with_error(con);
  }
}
