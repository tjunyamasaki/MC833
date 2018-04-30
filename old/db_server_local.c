// For more information, check http://zetcode.com/db/mysqlc/

//Includes std e sql
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <my_global.h>
#include <mysql.h>


// Funcoes auxiliares
void finish_with_error(MYSQL *con);
void login_server(MYSQL *con);
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
  while(1) {
    MYSQL *con = mysql_init(NULL);

    if (con == NULL)
    {
      fprintf(stderr, "%s\n", mysql_error(con));
      exit(1);
    }
    int menu_code;

    printf("\n--------------------------------------- \n");
    printf("  Bem vindo ao servidor de Disciplinas! \n");
    printf("--------------------------------------- \n\n");

    printf("Operacoes disponiveis:\n");
    printf(" 1 -> Login\n");
    printf("-1 -> Encerrar sessao\n");

    printf("\nSelecione opcao desejada:\n");
    scanf("%d", &menu_code);

    switch (menu_code)
    {
      case 1:
        printf("-- Operação login --\n\n");
        login_server(con);
        break;
      case 0:
        printf("Fechando conexao!\n");
        exit(0);
        break;
      default:
        printf("Operacao invalida. Selecione outra.\n");
        break;
    }
  }
  exit(0);
}

// *********************** Funcoes AUXILIARES *********************** //

void login_server(MYSQL *con)
{
  char username[25], password[25];

  printf("Enter username: ");
  scanf("%s", username);
  printf("Enter password: ");
  scanf("%s", password);

  if (mysql_real_connect(con, "localhost",
      username, password, "projeto1", 0, NULL, 0) == NULL)
  {
      finish_with_error(con);
  }

  // Descobre se username eh admin, professor ou aluno.
  char user_role[20];
  get_user_role(con, user_role, username);

  printf("\n--------------------------------------- \n");
  printf("Bem vindo %s '%s'!\n", user_role, username);
  printf("--------------------------------------- \n\n");

  int login=1;
  while(login == 1)
  {
    int op_code;

    scanf("%d", &op_code);

    // Usuario eh Admin do BD.
    if(strcmp(user_role, "admin") == 0)
    {
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
  char table[2500];

  MYSQL_ROW row;
  MYSQL_FIELD *field;

  strcat(table,"--------------------------------------- \n");
  while(field = mysql_fetch_field(result))
  {
     //printf("%s  ", field->name);
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

  printf("%s", table);

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
