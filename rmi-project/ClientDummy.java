import java.rmi.*;

public class BancoDiscClient {
	public static void main(String[] args) {
		try {
			Remote robj = Naming.lookup("//192.168.0.9/BancoDiscServer");
			WeatherIntf server = (BancoDiscInterface) robj;

			String forecast = server.getWeather();

			System.out.println("The weather will be " + forecast); 

		} catch (Exception e) {System.out.println(e.getMessage()); }
	}
} 



int login;
	String input[5];

	System.out.println("\n********************************************************\n");
	System.out.println("\t BEM VINDO AO BANCO DE DISCIPLINAS!!\n");
	System.out.println("********************************************************\n");

	do {
		print_tela_inicial();
		System.out.println("Selecione uma opcao:\n");

		get_input(input, sizeof(input));
		login = atoi(input);
		write_buffer(sockfd, input);  // Envia codigo do login

		switch(login)
		{
			case 1: // Professor
				professor(sockfd);
				break;
			case 2: // Aluno
				aluno(sockfd);
				break;
			case 0: // Exit
				System.out.println("\nFechando conexao.\n");
				break;
			default:
				System.out.println("\nOperacao Inval¡da.\n");
		}
	} while(login);
}

void professor(int sockfd)
{
	int choice;
	String opcode[5];

	System.out.println("\n-------------------------------------------------------\n");
	System.out.println("\n\t\t*** Bem Vindo Professor! ***\n");

	do {
		print_ops_professor();

		System.out.println("Selecione uma operacao:\n");

		get_input(opcode, sizeof(opcode));
		choice = atoi(opcode);

		switch (choice)
		{
			case 1:
				list_codes();
				#function_time_eval(list_codes, sockfd, opcode);
				break;
			case 2:
				get_ementa();
				#function_time_eval(get_ementa, sockfd, opcode);
				break;
			case 3:
				get_comment();
				#function_time_eval(get_comment, sockfd, opcode);
				break;
			case 4:
				get_full_info();
				#function_time_eval(get_full_info, sockfd, opcode);
				break;
			case 5:
				get_all_info();
				#function_time_eval(get_all_info, sockfd, opcode);
				break;
			case 6:
				write_comment();
				#function_time_eval(write_comment, sockfd, opcode);
				break;
			case 0:
				System.out.println("\nProfessor logging out...\n");
				break;
			default:
				System.out.println("\nInvalid Op Code!\n");
			}
	} while(choice);
}

void aluno(int sockfd)
{
	int choice;
	String opcode[5];

	System.out.println("\n-------------------------------------------------------\n");
	System.out.println("\n\t\t*** Bem Vindo Aluno! ***\n");

	do {
		print_ops_aluno();

		System.out.println("Selecione uma operacao:\n");
		get_input(opcode, sizeof(opcode));
		choice = atoi(opcode);

		switch (choice)
		{
			case 1:
				list_codes();
				break;
			case 2:
				get_ementa();
				break;
			case 3:
				get_comment();
				break;
			case 4:
				get_full_info();
				break;
			case 5:
				get_all_info();
				break;
			case 0:
				System.out.println("\nAluno logging out...\n");
				break;
			default:
				System.out.println("\nInvalid Op Code.\n");
		}
	} while(choice);
}

# *********** Operacoes de ALUNO e PROFESSOR *********** #

# Listar todos os códigos de disciplinas com seus respectivos títulos;
void list_codes(int sockfd, String opcode[5])
{
	String result;

	
	System.out.println("\n-------------------------------------------------------\n");
	System.out.println(" 1 -> Listar codigos das disciplinas\n");
	System.out.println("-------------------------------------------------------\n\n");

	result = server.list_codes();
	
	# Recebe Resultado
	System.out.println(result);
	System.out.println("********************************************************\n");
}

# Dado o código de uma disciplina, retornar a ementa;
void get_ementa(int sockfd, String opcode)
{
	String result[2500];
	String search_code[10];


	System.out.println("\n-------------------------------------------------------\n");
	System.out.println(" 2 -> Buscar ementa\n");
	System.out.println("-------------------------------------------------------\n\n");

	System.out.println("Digite o codigo da disciplina desejada:\n");
	get_input(search_code, sizeof(search_code));
	write_buffer(sockfd, search_code);

	System.out.println("\n");
	read_buffer(sockfd, result);
	System.out.println("%s", result);
	System.out.println("********************************************************\n");
}

# Dado o código de uma disciplina, retornar o texto de comentário sobre a próxima aula.
void get_comment(int sockfd, String opcode)
{
	String result;
	String search_code;


	System.out.println("\n-------------------------------------------------------\n");
	System.out.println(" 3 -> Buscar comentario sobre a proxima aula\n");
	System.out.println("-------------------------------------------------------\n\n");

	System.out.println("Digite o codigo da disciplina desejada:\n");
	get_input(search_code, sizeof(search_code));

	write_buffer(sockfd, search_code);

	System.out.println("\n");
	read_buffer(sockfd, result);
	System.out.println("%s", result);
	System.out.println("********************************************************\n");
}

# Dado o código de uma disciplina, retornar todas as informações desta disciplina;
void get_full_info(int sockfd, String opcode[5])
{
	String result[2500];
	String search_code[10];

	// Envia OP Code
	write_buffer(sockfd, opcode);

	System.out.println("\n-------------------------------------------------------\n");
	System.out.println(" 4 -> Listar informacoes de uma disciplina\n");
	System.out.println("-------------------------------------------------------\n\n");

	System.out.println("Digite o codigo da disciplina desejada:\n");

	get_input(search_code, sizeof(search_code));
	write_buffer(sockfd, search_code);

	System.out.println("\n");
	read_buffer(sockfd, result);
	System.out.println("%s", result);
	System.out.println("********************************************************\n");
}

# Listar todas as informações de todas as disciplinas
void get_all_info(int sockfd, String opcode[5])
{
	String result;

	# Envia OP Code
	write_buffer(sockfd, opcode);

	System.out.println("\n-------------------------------------------------------\n");
	System.out.println(" 5 -> Listar informacoes de todas as disciplinas\n");
	System.out.println("-------------------------------------------------------\n\n");

	read_buffer(sockfd, result);
	System.out.println(result);
	System.out.println("********************************************************\n");
}

# *********** Operacoes do PROFESSOR *********** #

# Escrever um texto de comentário sobre a próxima aula de uma disciplina (apenas usuário professor)
void write_comment(int sockfd, String opcode[5])
{
	String search_code, comment, response;
	int result;

	// Envia OP Code
	write_buffer(sockfd, opcode);

	System.out.println("\n-------------------------------------------------------\n");
	System.out.println(" 6 -> Escrever comentario sobre a proxima aula de uma disciplina\n");
	System.out.println("-------------------------------------------------------\n\n");

	System.out.println("Digite o codigo da disciplina desejada:\n");

	get_input(search_code, sizeof(search_code));
	write_buffer(sockfd, search_code);

	System.out.println("\nDigite o comentario que deseja inserir em %s:\n", search_code);

	get_input(comment, sizeof(comment));
	write_buffer(sockfd, comment);

	read_buffer(sockfd, response);
	result = atoi(response);
	if(result)
	{
		System.out.println("\n*** Comentario adicionado!! ***\n");
	}
	else
	{
		System.out.println("\n*** Falha ao tentar adicionar comentario! ***\n");
	}

	System.out.println("\n********************************************************\n");
}

# *********** Prints *********** #


void print_tela_inicial()
{
	System.out.println("\n-------------------------------------------------------\n");
	System.out.println("Opçoes de login disponiveis:\n");
	System.out.println("-------------------------------------------------------\n\n");

	System.out.println("1. Login Professor\n");
	System.out.println("2. Login Aluno\n");
	System.out.println("0. Exit\n");
	System.out.println("\n-------------------------------------------------------\n\n");
}

void print_ops_professor()
{
	System.out.println("\n-------------------------------------------------------\n");
	System.out.println("Operacoes disponiveis:\n");
	System.out.println("-------------------------------------------------------\n\n");
	System.out.println(" 0. Logout\n");
	System.out.println(" 1. Listar codigos das disciplinas\n");
	System.out.println(" 2. Buscar ementa\n");
	System.out.println(" 3. Buscar comentario sobre a proxima aula\n");
	System.out.println(" 4. Listar informacoes de uma disciplina\n");
	System.out.println(" 5. Listar informacoes de todas as disciplinas\n");
	System.out.println(" 6. Escrever comentario sobre a proxima aula de uma disciplina\n");
	System.out.println("\n-------------------------------------------------------\n\n");
}

void print_ops_aluno()
{
	System.out.println("\n-------------------------------------------------------\n");
	System.out.println("Operacoes disponiveis:\n");
	System.out.println("-------------------------------------------------------\n\n");
	System.out.println(" 0. Logout\n");
	System.out.println(" 1. Listar codigos das disciplinas\n");
	System.out.println(" 2. Buscar ementa\n");
	System.out.println(" 3. Buscar comentario sobre a proxima aula\n");
	System.out.println(" 4. Listar informacoes de uma disciplina\n");
	System.out.println(" 5. Listar informacoes de todas as disciplinas\n");
	System.out.println("\n-------------------------------------------------------\n\n");
}
