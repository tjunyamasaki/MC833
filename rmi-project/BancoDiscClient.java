import java.rmi.AccessException;
import java.rmi.NotBoundException;
import java.rmi.RemoteException;
import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;
import java.util.Scanner;

import java.io.*;


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

    	System.out.println("-------------------------------------------------------");
    	System.out.println(" 1 -> Listar codigos das disciplinas");
    	System.out.println("-------------------------------------------------------\n");

      String response = null;

      try {
        long before = System.currentTimeMillis();
        response = stub.list_codes();
        long dif = System.currentTimeMillis() - before;
        System.out.println("Time: " + dif);

        try{
          Writer fileWriter = new FileWriter("client_operation_1.txt", true);
          fileWriter.write(dif + "\n");
          fileWriter.close();
        }
        catch(Exception e){
          System.out.println("Deu erro!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
        }

  		} catch (RemoteException e) {
  			// TODO Auto-generated catch block
  			e.printStackTrace();
  		}
      System.out.println(response);
    }

    public void get_ementa(BancoDisciplinas stub, Scanner reader) {

    	System.out.println("-------------------------------------------------------");
    	System.out.println(" 2 -> Buscar ementa");
    	System.out.println("-------------------------------------------------------\n");

    	System.out.println("Digite o codigo da disciplina desejada:");
      Scanner buf = new Scanner(System.in);
    	String userInput = buf.nextLine();

      String response = null;

      try {
        long before = System.currentTimeMillis() ;
        response = stub.get_ementa(userInput);
        long after = System.currentTimeMillis() ;

        long dif = after - before;

        try{
          Writer fileWriter = new FileWriter("client_operation_2.txt", true);
          fileWriter.write(dif + "\n");
          fileWriter.close();
        }
        catch(Exception e){
          System.out.println("Deu erro!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
        }

        System.out.println("Time: " + dif);
  		} catch (RemoteException e) {
  			// TODO Auto-generated catch block
  			e.printStackTrace();
  		}
      System.out.println(response);

    }

    public void get_comment(BancoDisciplinas stub, Scanner reader) {

    	System.out.println("-------------------------------------------------------");
    	System.out.println(" 3 -> Buscar comentario sobre a proxima aula");
    	System.out.println("-------------------------------------------------------\n");

    	System.out.println("Digite o codigo da disciplina desejada:");

      Scanner buf = new Scanner(System.in);
    	String userInput = buf.nextLine();

      String response = null;
  		try {
        long before = System.currentTimeMillis() ;
        response = stub.get_comment(userInput);
        long after = System.currentTimeMillis() ;

        long dif = after - before;

        try{
          Writer fileWriter = new FileWriter("client_operation_3.txt", true);
          fileWriter.write(dif + "\n");
          fileWriter.close();
        }
        catch(Exception e){
          System.out.println("Deu erro!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
        }

        System.out.println("Time: " + dif);
  		} catch (RemoteException e) {
  			// TODO Auto-generated catch block
  			e.printStackTrace();
  		}
      System.out.println(response);
    }

    public void get_full_info(BancoDisciplinas stub, Scanner reader) {

    	System.out.println("-------------------------------------------------------");
    	System.out.println(" 4 -> Listar informacoes de uma disciplina");
    	System.out.println("-------------------------------------------------------\n");

    	System.out.println("Digite o codigo da disciplina desejada:");
      Scanner buf = new Scanner(System.in);
    	String userInput = buf.nextLine();

      String response = null;
  		try {
        long before = System.currentTimeMillis() ;
        response = stub.get_full_info(userInput);
        long after = System.currentTimeMillis() ;

        long dif = after - before;

        try{
          Writer fileWriter = new FileWriter("client_operation_4.txt", true);
          fileWriter.write(dif + "\n");
          fileWriter.close();
        }
        catch(Exception e){
          System.out.println("Deu erro!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
        }

        System.out.println("Time: " + dif);
  		} catch (RemoteException e) {
  			// TODO Auto-generated catch block
  			e.printStackTrace();
  		}
    	System.out.println(response);
    }

    public void get_all_info(BancoDisciplinas stub) {

    	System.out.println("-------------------------------------------------------");
    	System.out.println(" 5 -> Listar informacoes de todas as disciplinas");
    	System.out.println("-------------------------------------------------------\n");


      String response = null;
  		try {
        long before = System.currentTimeMillis() ;
        response = stub.get_all_info();
        long after = System.currentTimeMillis() ;

        long dif = after - before;

        try{
          Writer fileWriter = new FileWriter("client_operation_5.txt", true);
          fileWriter.write(dif + "\n");
          fileWriter.close();
        }
        catch(Exception e){
          System.out.println("Deu erro!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
        }

        System.out.println("Time: " + dif);
  		} catch (RemoteException e) {
  			// TODO Auto-generated catch block
  			e.printStackTrace();
  		}
      System.out.println(response);
    }

    // Operacoes dos professores
    public void write_comment(BancoDisciplinas stub, Scanner reader) {

    	System.out.println("-------------------------------------------------------");
    	System.out.println(" 6 -> Escrever comentario sobre a proxima aula de uma disciplina");
    	System.out.println("-------------------------------------------------------\n");


    	System.out.println("Digite o codigo da disciplina desejada:");
      Scanner buf = new Scanner(System.in);
    	String search_code = buf.nextLine();

    	System.out.println("Digite o comentario que deseja inserir em" + search_code + ":");
    	String comment = buf.nextLine();

      int response = 0;

      // System.out.println(search_code);
      // System.out.println(comment);
  		try {
        long before = System.currentTimeMillis() ;
        response = stub.write_comment(search_code, comment);
        long after = System.currentTimeMillis() ;

        long dif = after - before;

        try{
          Writer fileWriter = new FileWriter("client_operation_6.txt", true);
          fileWriter.write(dif + "\n");
          fileWriter.close();
        }
        catch(Exception e){
          System.out.println("Deu erro!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
        }

        System.out.println("Time: " + dif);
  		} catch (RemoteException e) {
  			// TODO Auto-generated catch block
  			e.printStackTrace();
  		}

      if(response==1){
        System.out.println("Comentario adicionado com sucesso.");
      }
      else{
        System.out.println("Houve algum erro ao tentar adicionar o comentario");
      }
    }

    private static void print_tela_inicial()
    {
    	System.out.println("-------------------------------------------------------");
    	System.out.println("Opcoes de login disponiveis:");
    	System.out.println("-------------------------------------------------------\n");

    	System.out.println("1. Login Professor");
    	System.out.println("2. Login Aluno");
    	System.out.println("0. Exit");
    	System.out.println("-------------------------------------------------------\n");
    }


    private static void print_ops_professor()
  	{
  		System.out.println("-------------------------------------------------------");
  		System.out.println("Operacoes disponiveis:");
  		System.out.println("-------------------------------------------------------\n");
  		System.out.println(" 0. Logout");
  		System.out.println(" 1. Listar codigos das disciplinas");
  		System.out.println(" 2. Buscar ementa");
  		System.out.println(" 3. Buscar comentario sobre a proxima aula");
  		System.out.println(" 4. Listar informacoes de uma disciplina");
  		System.out.println(" 5. Listar informacoes de todas as disciplinas");
  		System.out.println(" 6. Escrever comentario sobre a proxima aula de uma disciplina");
  		System.out.println("-------------------------------------------------------\n");
  	}

    private static void print_ops_aluno()
  	{
  		System.out.println("-------------------------------------------------------");
  		System.out.println("Operacoes disponiveis:");
  		System.out.println("-------------------------------------------------------\n");
  		System.out.println(" 0. Logout");
  		System.out.println(" 1. Listar codigos das disciplinas");
  		System.out.println(" 2. Buscar ementa");
  		System.out.println(" 3. Buscar comentario sobre a proxima aula");
  		System.out.println(" 4. Listar informacoes de uma disciplina");
  		System.out.println(" 5. Listar informacoes de todas as disciplinas");
  		System.out.println("-------------------------------------------------------\n");
  	}

    public static void main(String[] args) {

        Scanner reader = new Scanner(System.in);

        System.setProperty("java.security.policy","file:C:/Users/tjuny/Desktop/UNICAMP/MC833/rmi-project/security.policy");
        if (System.getSecurityManager() == null) {
            System.setSecurityManager(new SecurityManager());
        }

        String host = (args.length < 1) ? null : args[0];
        try {
            Registry registry = LocateRegistry.getRegistry(host);

            System.out.println("\n\n********************************************************");
          	System.out.println("\t BEM VINDO AO BANCO DE DISCIPLINAS!!");
          	System.out.println("********************************************************");

          	int login = 1;
          	int choice = 1;

          	BancoDiscClient client = new BancoDiscClient();
            BancoDisciplinas stub = (BancoDisciplinas) registry.lookup("BancoDisciplinas");

            //  = (BancoDisciplinas) registry.lookup("BancoDisciplinas");

          	while(login != 0) {

          		print_tela_inicial();

          		System.out.println("Selecione uma opcao:");

              // Scanner reader = new Scanner(System.in);

          		login = reader.nextInt();

          		// reader.close();

          		switch(login)
          		{
          			case 1: // Professor

          				System.out.println("\n\n-------------------------------------------------------");
          				System.out.println("\t\t*** Bem Vindo Professor! ***");
          				System.out.println("-------------------------------------------------------");

          				while(choice != 0) {
          					print_ops_professor();

          					System.out.println("Selecione uma operacao:");

                    // Scanner profChoice = new Scanner(System.in);
                    choice = reader.nextInt();
                    // profChoice.close();

          					switch (choice)
          					{
          						case 1:
          							client.list_codes(stub);
  //        							function_time_eval(list_codes, sockfd, opcode);
          							break;
          						case 2:
          							client.get_ementa(stub, reader);
  //        							function_time_eval(get_ementa, sockfd, opcode);
          							break;
          						case 3:
          							client.get_comment(stub, reader);
  //        							function_time_eval(get_comment, sockfd, opcode);
          							break;
          						case 4:
          							client.get_full_info(stub, reader);
  //        							function_time_eval(get_full_info, sockfd, opcode);
          							break;
          						case 5:
          							client.get_all_info(stub);
  //        							function_time_eval(get_all_info, sockfd, opcode);
          							break;
          						case 6:
          							client.write_comment(stub, reader);
  //        							function_time_eval(write_comment, sockfd, opcode);
          							break;
          						case 0:
          							System.out.println("\n\nProfessor logging out...");
          							break;
          						default:
          							System.out.println("Invalid Op Code!");
          					}
          				}

                  choice = 1;
          				break;

          			case 2: // Aluno

          				System.out.println("\n\n-------------------------------------------------------");
          				System.out.println("\t\t*** Bem Vindo Aluno! ***");
          				System.out.println("-------------------------------------------------------");

          				while(choice != 0) {
          					print_ops_aluno();

          					// Scanner alunoChoice = new Scanner(System.in);
          					System.out.println("Selecione uma operacao:");

                    choice = reader.nextInt();

          					// alunoChoice.close();

          					switch (choice)
          					{
          						case 1:
          							client.list_codes(stub);
  //        							function_time_eval(list_codes, sockfd, opcode);
          							break;
          						case 2:
          							client.get_ementa(stub, reader);
  //        							function_time_eval(get_ementa, sockfd, opcode);
          							break;
          						case 3:
          							client.get_comment(stub, reader);
  //        							function_time_eval(get_comment, sockfd, opcode);
          							break;
          						case 4:
          							client.get_full_info(stub, reader);
  //        							function_time_eval(get_full_info, sockfd, opcode);
          							break;
          						case 5:
          							client.get_all_info(stub);
  //        							function_time_eval(get_all_info, sockfd, opcode);
          							break;
          						case 0:
          							System.out.println("\n\nAluno logging out...");
          							break;
          						default:
          							System.out.println("Invalid Op Code!");
          						}
          				}
                  choice = 1;
          				break;
          			case 0: // Exit
          				System.out.println("Fechando conexao.");
          				break;
          			default:
          				System.out.println("Operacao Invalida.");
          		}
          	}
        } catch (Exception e) {
            System.err.println("Client exception: " + e.toString());
            e.printStackTrace();
        }

        reader.close();
    }
}
