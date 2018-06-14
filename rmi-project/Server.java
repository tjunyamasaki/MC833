import java.rmi.registry.Registry;
import java.rmi.registry.LocateRegistry;
import java.rmi.RemoteException;
import java.rmi.server.UnicastRemoteObject;
import java.util.Scanner;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;

public class Server extends UnicastRemoteObject implements BancoDisciplinas {

    private Connection con;
    private PreparedStatement comando;
    private ResultSet resultado;
    //private String hostname="localhost"; // where you run the rmi registry
    //private int port = 1099; // the port of the rmiregistry (by default it takes 1099)
    //private String objectname="BancoDisciplinas"; // your object name in the naming service(rmiregistry)

    public Server() {

        try {
			String con = "jdbc:mysql://localhost:3306/projeto1";
	        String user = "professor";
	        String password = "123mudar";

		    conexao = DriverManager.getConnection(con, user, password);

		    if (!conexao.isClosed() ) {
		    	System.out.println(" CONECTED TO DATABASE! ");
		    }
		    else {
		    	System.out.println(" ERROR CONNECTING TO DATABASE! ");
		    }
        } catch (SQLException ex) {

    		    System.out.println("SQLException: " + ex.getMessage());
    		    System.out.println("SQLState: " + ex.getSQLState());
    		    System.out.println("VendorError: " + ex.getErrorCode());
    	}
    }

    // Aluno
    public String list_codes() throws RemoteException {

        String consulta = "SELECT CODIGO_DISCIPLINA AS CODIGO, TITULO"
                        + "FROM DISCIPLINAS;";

        comando = conexao.prepareStatement(consulta);

        if (comando.execute()); {
            resultado = comando.getResultSet();

            while (resultado.next()) {
               System.out.println("Codigo: " + resultado.getString("CODIGO") );
               System.out.println("Nome: " + resultado.getString("TITULO") );
               System.out.println();
            }
        }
        comando.clearParameters();
    }

    public String get_ementa(String search_code) throws RemoteException {

        String consulta = "SELECT EMENTA"
                        + "FROM DISCIPLINAS"
                        + "WHERE CODIGO_DISCIPLINA = ?;";

        comando = conexao.prepareStatement(consulta);

        comando.setString(1,search_code);

        if (comando.execute()); {
            resultado = comando.getResultSet();

            while (resultado.next()) {
               System.out.println("Ementa: " + resultado.getString("EMENTA") );
               System.out.println();
            }
        }
        comando.clearParameters();

    }

    public String get_comment(String search_code) throws RemoteException {

        String consulta = "SELECT COMENTARIO"
                        + "FROM DISCIPLINAS"
                        + "WHERE CODIGO_DISCIPLINA = ?;";

        comando = conexao.prepareStatement(consulta);

        comando.setString(1,search_code);

        if (comando.execute()); {
            resultado = comando.getResultSet();

            while (resultado.next()) {
               System.out.println("Comentario: " + resultado.getString("COMENTARIO") );
               System.out.println();
            }
        }
        comando.clearParameters();
    }

    public String get_full_info(String search_code) throws RemoteException {

        String consulta = "SELECT *"
                        + "FROM DISCIPLINAS"
                        + "WHERE CODIGO_DISCIPLINA = ?;";

        comando = conexao.prepareStatement(consulta);

        comando.setString(1,search_code);

        if (comando.execute()); {
            resultado = comando.getResultSet();

            while (resultado.next()) {
               System.out.println("Nome: " + resultado.getString("NOME_COMPLETO") );
               System.out.println("Titulo da Tese: " + resultado.getString("TITULO_DA_DISSERTACAO_TESE") );
               System.out.println();
            }
        }
        comando.clearParameters();
    }

    public String get_all_info() throws RemoteException {

        String consulta = "SELECT *"
                        + "FROM DISCIPLINAS;";

        comando = conexao.prepareStatement(consulta);

        if (comando.execute()); {
            resultado = comando.getResultSet();

            while (resultado.next()) {
               System.out.println("Nome: " + resultado.getString("NOME_COMPLETO") );
               System.out.println("Titulo da Tese: " + resultado.getString("TITULO_DA_DISSERTACAO_TESE") );
               System.out.println();
            }
        }
        comando.clearParameters();
    }

    // Professor
    public String write_comment(String search_code, String comment) throws RemoteException {

        String consulta = "UPDATE DISCIPLINAS"
                        + "SET COMENTARIO = ?"
                        + "WHERE CODIGO_DISCIPLINA = ?;";

        comando = conexao.prepareStatement(consulta);

        comando.setString(1,search_code);
        comando.setString(2,comment);

        if (comando.execute()); {
            resultado = comando.getResultSet();

            while (resultado.next()) {
               System.out.println("Nome: " + resultado.getString("NOME_COMPLETO") );
               System.out.println("Titulo da Tese: " + resultado.getString("TITULO_DA_DISSERTACAO_TESE") );
               System.out.println();
            }
        }
        comando.clearParameters();
    }

    // Inicia Server
    public static void main(String args[]) {

        if (System.getSecurityManager() == null) {
            System.setSecurityManager(new SecurityManager());
        }
        try {
            LocateRegistry.createRegistry(1099);
            Server server = new Server();
            BancoDisciplinas stub = (BancoDisciplinas) UnicastRemoteObject.exportObject(server, 0);

            // Bind the remote object's stub in the registry
            Registry registry = LocateRegistry.getRegistry(1099);
            registry.bind("BancoDisciplinas", stub);

            System.err.println("Server ready");
        } catch (Exception e) {
            System.err.println("Server exception: " + e.toString());
            e.printStackTrace();
        }

	    if(resultado != null) {
	    	resultado.close();
	    }
	    if(comando != null) {
	    	comando.close();
	    }
	    if(conexao != null) {
	    	conexao.close();
	    }
    }
}
