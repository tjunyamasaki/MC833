// For more information, check http://zetcode.com/db/mysqlc/
#include <stdio.h>
#include <string.h>
#include <my_global.h>
#include <mysql.h>

void finish_with_error(MYSQL *con);
void start_db(MYSQL *con);
void list_users(MYSQL *con);
void create_user(MYSQL *con);
void delete_user(MYSQL *con);

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

  printf("\n--------------------------------------- \n");
  printf("Bem vindo '%s' admin!\n", username);
  printf("--------------------------------------- \n");

  while(1)
  {
    int op_code;

    printf("\nOperacoes disponiveis:\n");
    printf(" 1 -> Inicializar BD\n");
    printf(" 2 -> Listar usuarios\n");
    printf(" 3 -> Criar usuario\n");
    printf(" 4 -> Deletar usuario\n");
    printf("-1 -> Finalizar sessao\n");
    printf("\nSelecione opcao desejada:\n");
    scanf("%d", &op_code);

    switch (op_code) {
      case 1:
        start_db(con);
        break;
      case 2:
        list_users(con);
        break;
      case 3:
        create_user(con);
        break;
      case 4:
        delete_user(con);
        break;
      case -1:
        mysql_close(con);
        exit(0);
    }
  }
  mysql_close(con);
  exit(0);
}

void finish_with_error(MYSQL *con)
{
  fprintf(stderr, "%s\n", mysql_error(con));
  mysql_close(con);
  exit(1);
}

void start_db(MYSQL *con)
{
  char querry_command[1000];

  strcpy(querry_command, "CREATE TABLE IF NOT EXISTS DISCIPLINAS(CODIGO_DISCIPLINA VARCHAR(5) NOT NULL, TITULO VARCHAR(250) NULL, EMENTA VARCHAR(1000) NULL, SALA VARCHAR(5) NULL, HORARIO VARCHAR(45) NULL, COMENTARIO VARCHAR(1500) NULL, PRIMARY KEY (CODIGO_DISCIPLINA))");

  if (mysql_query(con, querry_command)) {
      finish_with_error(con);
  }

  strcpy(querry_command, "CREATE TABLE IF NOT EXISTS USUARIOS(LOGIN VARCHAR(45) NOT NULL, TIPO VARCHAR(45) NULL, PRIMARY KEY (LOGIN))");

  if (mysql_query(con, querry_command)) {
      finish_with_error(con);
  }

  strcpy(querry_command, "LOAD DATA INFILE 'DATA_DIR/data/DISCIPLINAS.csv' IGNORE INTO TABLE DISCIPLINAS CHARACTER SET utf8 FIELDS TERMINATED BY ',' OPTIONALLY ENCLOSED BY '\"' LINES TERMINATED BY '\r\n' IGNORE 1 LINES (CODIGO_DISCIPLINA,TITULO,EMENTA,SALA,HORARIO,COMENTARIO);");

  if (mysql_query(con, querry_command)) {
      finish_with_error(con);
  }

  strcpy(querry_command, "LOAD DATA INFILE 'DATA_DIR/data/USUARIOS.csv' IGNORE INTO TABLE USUARIOS CHARACTER SET utf8 FIELDS TERMINATED BY ',' OPTIONALLY ENCLOSED BY '\"' LINES TERMINATED BY '\r\n' IGNORE 1 LINES (LOGIN, TIPO);");

  if (mysql_query(con, querry_command)) {
      finish_with_error(con);
  }
}

void list_users(MYSQL *con)
{
  char querry_command[250] = "SELECT User FROM mysql.user;";

  printf("--------------------------------------- \n");

  if (mysql_query(con, querry_command)) {
    finish_with_error(con);
  }

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
                printf("\n--------------------------------------- ");
             }
             printf("\n");
          }
          printf("%s  ", row[i] ? row[i] : "NULL");
      }
  }
  printf("\n");
  mysql_free_result(result);
  printf("--------------------------------------- \n");
}

void create_user(MYSQL *con)
{
  char username[25], password[25], querry_command[1000];
  int user_type;

  printf("--------------------------------------- \n");
  printf("\nDigite o username que sera criado:\n");
  scanf("%s", username);

  printf("Digite a password desejada:\n");
  scanf("%s", password);

  printf("\nTipos de usuario disponiveis:\n");
  printf("1 -> Aluno:\n");
  printf("2 -> Professor:\n");

  printf("Selecione o tipo:\n");
  scanf("%d", &user_type);

  switch (user_type) {
    case 1:
      strcpy(querry_command, "GRANT SELECT ON projeto1.* TO '");
      break;
    case 2:
      strcpy(querry_command, "GRANT SELECT, UPDATE ON projeto1.* TO '");
      break;
    default:
      printf("Tipo invalido. Abortando operacao.\n");
      return;
  }
  strcat(querry_command, username);
  strcat(querry_command, "'@'localhost' IDENTIFIED BY '");
  strcat(querry_command, password);
  strcat(querry_command, "';");

  if (mysql_query(con, querry_command)) {
    finish_with_error(con);
  }

  printf("--------------------------------------- \n");
}

void delete_user(MYSQL *con)
{
  char username[25], querry_command[1000];

  printf("--------------------------------------- \n");
  printf("\nDigite o username que sera deletado:\n");
  scanf("%s", username);

  strcpy(querry_command, "DROP USER '");
  strcat(querry_command, username);
  strcat(querry_command, "'@'localhost';");

  if (mysql_query(con, querry_command)) {
    finish_with_error(con);
  }

  printf("--------------------------------------- \n");
}
