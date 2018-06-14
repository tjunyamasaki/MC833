package teste1;

import java.rmi.AccessException;
import java.rmi.NotBoundException;
import java.rmi.RemoteException;
import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;
import java.util.Scanner;

public class BancoDiscClient {

    private BancoDiscClient() {}

//  Hello stub = (Hello) registry.lookup("Hello");
//	Scanner reader = new Scanner(System.in);  // Reading from System.in
//	System.out.println("Quantas vezes teremos que saudar o BancoDisciplinas hoje?");
//	int n = reader.nextInt(); // Scans the next token of the input as an int.
//	//once finished
//	reader.close();
//  String response = stub.saudarBancoDisciplinas(n);
//  System.out.println("response: " + response);


//    String list_codes() throws RemoteException;
//    String get_ementa(String search_code) throws RemoteException;
//    String get_comment(String search_code) throws RemoteException;
//    String get_full_info(String search_code) throws RemoteException;
//    String get_all_info() throws RemoteException;
//    // Professor
//    String write_comment(String search_code, String comment) throws RemoteException;

 // Operacoes dos alunos/professores
    public void list_codes(BancoDisciplinas stub) {

    	System.out.println("\n-------------------------------------------------------\n");
    	System.out.println(" 1 -> Listar codigos das disciplinas\n");
    	System.out.println("-------------------------------------------------------\n\n");

      String response = null;

  		try {
  			response = stub.list_codes();
  		} catch (RemoteException e) {
  			// TODO Auto-generated catch block
  			e.printStackTrace();
  		}
      System.out.println(response);

    }

    public void get_ementa(BancoDisciplinas stub) {

    	System.out.println("\n-------------------------------------------------------\n");
    	System.out.println(" 2 -> Buscar ementa\n");
    	System.out.println("-------------------------------------------------------\n\n");

    	Scanner reader = new Scanner(System.in);
    	System.out.println("Digite o codigo da disciplina desejada:\n");
    	String userInput = reader.nextLine();

  		reader.close();

  		String response = null;
  		try {
  			response = stub.get_ementa(userInput);
  		} catch (RemoteException e) {
  			// TODO Auto-generated catch block
  			e.printStackTrace();
  		}
      System.out.println(response);

    }

    public void get_comment(BancoDisciplinas stub) {

    	System.out.println("\n-------------------------------------------------------\n");
    	System.out.println(" 3 -> Buscar comentario sobre a proxima aula\n");
    	System.out.println("-------------------------------------------------------\n\n");

    	Scanner reader = new Scanner(System.in);
    	System.out.println("Digite o codigo da disciplina desejada:\n");
    	String userInput = reader.nextLine();
  		reader.close();

      String response = null;
  		try {
  			response = stub.get_comment(userInput);
  		} catch (RemoteException e) {
  			// TODO Auto-generated catch block
  			e.printStackTrace();
  		}
      System.out.println(response);
    }

    public void get_full_info(BancoDisciplinas stub) {

    	System.out.println("\n-------------------------------------------------------\n");
    	System.out.println(" 4 -> Listar informacoes de uma disciplina\n");
    	System.out.println("-------------------------------------------------------\n\n");

    	Scanner reader = new Scanner(System.in);
    	System.out.println("Digite o codigo da disciplina desejada:\n");
    	String userInput = reader.nextLine();
  		reader.close();

      String response = null;
  		try {
  			response = stub.get_full_info(userInput);
  		} catch (RemoteException e) {
  			// TODO Auto-generated catch block
  			e.printStackTrace();
  		}
    	System.out.println(response);

    }

    public void get_all_info(BancoDisciplinas stub) {

    	System.out.println("\n-------------------------------------------------------\n");
    	System.out.println(" 5 -> Listar informacoes de todas as disciplinas\n");
    	System.out.println("-------------------------------------------------------\n\n");

    	String response = null;
  		try {
  			response = stub.get_all_info();
  		} catch (RemoteException e) {
  			// TODO Auto-generated catch block
  			e.printStackTrace();
  		}
      System.out.println(response);

    }

    // Operacoes dos professores
    public void write_comment(BancoDisciplinas stub) {

    	System.out.println("\n-------------------------------------------------------\n");
    	System.out.println(" 6 -> Escrever comentario sobre a proxima aula de uma disciplina\n");
    	System.out.println("-------------------------------------------------------\n\n");


    	Scanner reader = new Scanner(System.in);

    	System.out.println("Digite o codigo da disciplina desejada:\n");
    	String search_code = reader.nextLine();

    	System.out.println("\nDigite o comentario que deseja inserir em" + search_code + ":\n");
    	String comment = reader.nextLine();

		  reader.close();

    	String response = null;
  		try {
  			response = stub.write_comment(search_code, comment);
  		} catch (RemoteException e) {
  			// TODO Auto-generated catch block
  			e.printStackTrace();
  		}
    	System.out.println(response);
    }

    private static void print_tela_inicial()
    {
    	System.out.println("\n-------------------------------------------------------\n");
    	System.out.println("Opçoes de login disponiveis:\n");
    	System.out.println("-------------------------------------------------------\n\n");

    	System.out.println("1. Login Professor\n");
    	System.out.println("2. Login Aluno\n");
    	System.out.println("0. Exit\n");
    	System.out.println("\n-------------------------------------------------------\n\n");
    }


    private static void print_ops_professor()
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

    private static void print_ops_aluno()
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

    public static void main(String[] args) {

        String host = (args.length < 1) ? null : args[0];
        try {
            Registry registry = LocateRegistry.getRegistry(host);

            System.out.println("\n********************************************************\n");
          	System.out.println("\t BEM VINDO AO BANCO DE DISCIPLINAS!!\n");
          	System.out.println("********************************************************\n");

          	int login = 1;
          	int choice = 1;

          	BancoDiscClient client = new BancoDiscClient();
            BancoDisciplinas stub = (BancoDisciplinas) registry.lookup("BancoDisciplinas");

          	while(login != 0) {

          		print_tela_inicial();
          		Scanner reader = new Scanner(System.in);
          		System.out.println("Selecione uma opcao:\n");
          		login = reader.nextInt();
          		reader.close();

          		switch(login)
          		{
          			case 1: // Professor

          				System.out.println("\n-------------------------------------------------------\n");
          				System.out.println("\n\t\t*** Bem Vindo Professor! ***\n");
          				System.out.println("\n-------------------------------------------------------\n");

          				while(choice != 0) {
          					print_ops_professor();

          					Scanner profChoice = new Scanner(System.in);
          					System.out.println("Selecione uma operacao:\n");
          					choice = profChoice.nextInt();
          					profChoice.close();

          					switch (choice)
          					{
          						case 1:
          							client.list_codes(stub);
  //        							function_time_eval(list_codes, sockfd, opcode);
          							break;
          						case 2:
          							client.get_ementa(stub);
  //        							function_time_eval(get_ementa, sockfd, opcode);
          							break;
          						case 3:
          							client.get_comment(stub);
  //        							function_time_eval(get_comment, sockfd, opcode);
          							break;
          						case 4:
          							client.get_full_info(stub);
  //        							function_time_eval(get_full_info, sockfd, opcode);
          							break;
          						case 5:
          							client.get_all_info(stub);
  //        							function_time_eval(get_all_info, sockfd, opcode);
          							break;
          						case 6:
          							client.write_comment(stub);
  //        							function_time_eval(write_comment, sockfd, opcode);
          							break;
          						case 0:
          							System.out.println("\nProfessor logging out...\n");
          							break;
          						default:
          							System.out.println("\nInvalid Op Code!\n");
          					}
          				}
          				break;

          			case 2: // Aluno

          				System.out.println("\n-------------------------------------------------------\n");
          				System.out.println("\n\t\t*** Bem Vindo Aluno! ***\n");
          				System.out.println("\n-------------------------------------------------------\n");

          				while(choice != 0) {
          					print_ops_aluno();

          					Scanner alunoChoice = new Scanner(System.in);
          					System.out.println("Selecione uma operacao:\n");
          					choice = alunoChoice.nextInt();
          					alunoChoice.close();

          					switch (choice)
          					{
          						case 1:
          							client.list_codes(stub);
  //        							function_time_eval(list_codes, sockfd, opcode);
          							break;
          						case 2:
          							client.get_ementa(stub);
  //        							function_time_eval(get_ementa, sockfd, opcode);
          							break;
          						case 3:
          							client.get_comment(stub);
  //        							function_time_eval(get_comment, sockfd, opcode);
          							break;
          						case 4:
          							client.get_full_info(stub);
  //        							function_time_eval(get_full_info, sockfd, opcode);
          							break;
          						case 5:
          							client.get_all_info(stub);
  //        							function_time_eval(get_all_info, sockfd, opcode);
          							break;
          						case 0:
          							System.out.println("\nAluno logging out...\n");
          							break;
          						default:
          							System.out.println("\nInvalid Op Code!\n");
          						}
          				}

          				break;
          			case 0: // Exit
          				System.out.println("\nFechando conexao.\n");
          				break;
          			default:
          				System.out.println("\nOperacao Invalida.\n");
          		}
          	}
        } catch (Exception e) {
            System.err.println("Client exception: " + e.toString());
            e.printStackTrace();
        }
    }
}
