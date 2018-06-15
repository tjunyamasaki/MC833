import java.rmi.registry.Registry;
import java.rmi.registry.LocateRegistry;
import java.rmi.RemoteException;
import java.rmi.server.UnicastRemoteObject;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.ResultSetMetaData;
import java.sql.SQLException;

import java.io.*;


public class Server implements BancoDisciplinas {

    public Server() {
        super();
    }

    private Connection connect() throws RemoteException, SQLException {
        String driver = "com.mysql.jdbc.Driver";
        String servidor = "jdbc:mysql://localhost:3306/projeto1";
        String usuario = "professor";
        String senha = "senha123";

        try {
            Class.forName(driver).newInstance();
            Connection con = DriverManager.getConnection(servidor, usuario, senha);
            return con;
        } catch(SQLException ex) {
            System.out.println("SQLException: " + ex.getMessage());
            System.out.println("SQLState: " + ex.getSQLState());
            System.out.println("VendorError: " + ex.getErrorCode());
            return null;
        } catch(Exception e) {
            System.out.println(e.getMessage());
            return null;
        }
    }

    // Aluno
    public String list_codes() throws RemoteException {
        try {
            Writer fileWriter = new FileWriter("server_operation_1.txt", true);
            long startTime = System.currentTimeMillis();

            Connection conexao = connect();

            String consulta = "SELECT CODIGO_DISCIPLINA, TITULO "
                            + "FROM DISCIPLINAS;";

            PreparedStatement comando = conexao.prepareStatement(consulta);

            ResultSet resultado = comando.executeQuery();

            ResultSetMetaData rsmd = resultado.getMetaData();

            StringBuilder resultString = new StringBuilder();

            for(int i = 1; i <= rsmd.getColumnCount(); i++) {
                resultString.append(rsmd.getColumnName(i));
                if(i == rsmd.getColumnCount()) {
                    resultString.append("\n");
                } else {
                    resultString.append(", ");
                }
            }

            while (resultado.next()) {
               resultString.append(resultado.getString(1) + ", ");
               resultString.append(resultado.getString(2) + "\n");
            }

            comando.clearParameters();
            resultado.close();
            comando.close();
            conexao.close();

            long diffTime = System.currentTimeMillis() - startTime;
            fileWriter.write(diffTime + "\n");
            fileWriter.close();

            return resultString.toString();

        } catch(SQLException ex) {
           System.out.println(ex.getMessage());
           return null;
        } catch(Exception e){
           System.out.println(e.getMessage());
           return null;
        }
    }

    public String get_ementa(String search_code) throws RemoteException, SQLException {
        try {
            Writer fileWriter = new FileWriter("server_operation_2.txt", true);
            long startTime = System.currentTimeMillis();

            Connection conexao = connect();

            String consulta = "SELECT EMENTA "
                            + "FROM DISCIPLINAS "
                            + "WHERE CODIGO_DISCIPLINA = ? ;";

            PreparedStatement comando = conexao.prepareStatement(consulta);

            comando.setString(1,search_code);

            ResultSet resultado = comando.executeQuery();

            StringBuilder resultString = new StringBuilder();

            while (resultado.next()) {
               resultString.append("Ementa: " + resultado.getString(1) + "\n");
            }

            comando.clearParameters();
            resultado.close();
            comando.close();
            conexao.close();

            long diffTime = System.currentTimeMillis() - startTime;
            fileWriter.write(diffTime + "\n");
            fileWriter.close();

            return resultString.toString();

        } catch(SQLException ex) {
            System.out.println("SQLException: " + ex.getMessage());
            System.out.println("SQLState: " + ex.getSQLState());
            System.out.println("VendorError: " + ex.getErrorCode());
            return null;
        } catch(Exception e){
           System.out.println(e.getMessage());
           return null;
        }
    }

    public String get_comment(String search_code) throws RemoteException, SQLException {
        try {
            Writer fileWriter = new FileWriter("server_operation_3.txt", true);
            long startTime = System.currentTimeMillis();

            Connection conexao = connect();

            String consulta = "SELECT COMENTARIO "
                            + "FROM DISCIPLINAS "
                            + "WHERE CODIGO_DISCIPLINA = ? ;";

            PreparedStatement comando = conexao.prepareStatement(consulta);

            comando.setString(1,search_code);

            ResultSet resultado = comando.executeQuery();

            StringBuilder resultString = new StringBuilder();

            while (resultado.next()) {
                resultString.append("Comentario: " + resultado.getString(1) + "\n");
            }

            comando.clearParameters();
            resultado.close();
            comando.close();
            conexao.close();

            long diffTime = System.currentTimeMillis() - startTime;
            fileWriter.write(diffTime + "\n");
            fileWriter.close();

            return resultString.toString();

        } catch(SQLException ex) {
            System.out.println("SQLException: " + ex.getMessage());
            System.out.println("SQLState: " + ex.getSQLState());
            System.out.println("VendorError: " + ex.getErrorCode());
            return null;
        } catch(Exception e){
           System.out.println(e.getMessage());
           return null;
        }
    }

    public String get_full_info(String search_code) throws RemoteException, SQLException {
        try {
            Writer fileWriter = new FileWriter("server_operation_4.txt", true);
            long startTime = System.currentTimeMillis();

            Connection conexao = connect();

            String consulta = "SELECT * "
                            + "FROM DISCIPLINAS "
                            + "WHERE CODIGO_DISCIPLINA = ? ;";

            PreparedStatement comando = conexao.prepareStatement(consulta);

            comando.setString(1,search_code);

            System.out.println("String para buscar:" + search_code);

            ResultSet resultado = comando.executeQuery();

            StringBuilder resultString = new StringBuilder();

            while (resultado.next()) {
                resultString.append("Codigo: " + resultado.getString(1) + "\n");
                resultString.append("Titulo: " + resultado.getString(2) + "\n");
                resultString.append("Ementa: " + resultado.getString(3) + "\n");
                resultString.append("Sala: " + resultado.getString(4) + "\n");
                resultString.append("Horario: " + resultado.getString(5) + "\n");
                resultString.append("Comentario: " + resultado.getString(6) + "\n");
            }

            comando.clearParameters();
            resultado.close();
            comando.close();
            conexao.close();

            System.out.println("Resultado:" + resultString.toString());

            long diffTime = System.currentTimeMillis() - startTime;
            fileWriter.write(diffTime + "\n");
            fileWriter.close();

            return resultString.toString();

        } catch(SQLException ex) {
            System.out.println("SQLException: " + ex.getMessage());
            System.out.println("SQLState: " + ex.getSQLState());
            System.out.println("VendorError: " + ex.getErrorCode());
            return null;
        } catch(Exception e){
           System.out.println(e.getMessage());
           return null;
        }
    }

    public String get_all_info() throws RemoteException, SQLException {
        try {
            Writer fileWriter = new FileWriter("server_operation_5.txt", true);
            long startTime = System.currentTimeMillis();

            Connection conexao = connect();

            String consulta = "SELECT * "
                            + "FROM DISCIPLINAS;";

            PreparedStatement comando = conexao.prepareStatement(consulta);

            ResultSet resultado = comando.executeQuery();

            ResultSetMetaData rsmd = resultado.getMetaData();

            StringBuilder resultString = new StringBuilder();

            for(int i = 1; i <= rsmd.getColumnCount(); i++) {
                resultString.append(rsmd.getColumnName(i));
                if(i == rsmd.getColumnCount()) {
                    resultString.append("\n");
                } else {
                    resultString.append(", ");
                }
            }

            while (resultado.next()) {
               resultString.append(resultado.getString(1) + ", " );
               resultString.append(resultado.getString(2) + ", " );
               resultString.append(resultado.getString(3) + ", " );
               resultString.append(resultado.getString(4) + ", " );
               resultString.append(resultado.getString(5) + ", " );
               resultString.append(resultado.getString(6) + "\n" );
            }

            comando.clearParameters();
            resultado.close();
            comando.close();
            conexao.close();

            long diffTime = System.currentTimeMillis() - startTime;
            fileWriter.write(diffTime + "\n");
            fileWriter.close();

            return resultString.toString();

        } catch(SQLException ex) {
            System.out.println("SQLException: " + ex.getMessage());
            System.out.println("SQLState: " + ex.getSQLState());
            System.out.println("VendorError: " + ex.getErrorCode());
            return null;
        } catch(Exception e){
           System.out.println(e.getMessage());
           return null;
        }
    }

    // Professor
    public int write_comment(String search_code, String comment) throws RemoteException, SQLException {
        try {
            Writer fileWriter = new FileWriter("server_operation_6.txt", true);
            long startTime = System.currentTimeMillis();

            Connection conexao = connect();

            String consulta = "UPDATE DISCIPLINAS "
                            + "SET COMENTARIO = ? "
                            + "WHERE CODIGO_DISCIPLINA = ? ;";

            PreparedStatement comando = conexao.prepareStatement(consulta);

            comando.setString(1,comment);
            comando.setString(2,search_code);

            int resultado = comando.executeUpdate();

            comando.clearParameters();
            comando.close();
            conexao.close();

            long diffTime = System.currentTimeMillis() - startTime;
            fileWriter.write(diffTime + "\n");
            fileWriter.close();

            return resultado;

        } catch(SQLException ex) {
            System.out.println("SQLException: " + ex.getMessage());
            System.out.println("SQLState: " + ex.getSQLState());
            System.out.println("VendorError: " + ex.getErrorCode());
            return 0;
        } catch(Exception e){
           System.out.println(e.getMessage());
           return 0;
        }
    }

    // Inicia Server
    public static void main(String args[]) {

        System.setProperty("java.security.policy","file:/Users/marcioivan/Dropbox/7ºSemestre/MC833/Projetos/rmi-project/security.policy");
        if (System.getSecurityManager() == null) {
            System.setSecurityManager(new SecurityManager());
        }

        try {
            Server server = new Server();
            BancoDisciplinas stub = (BancoDisciplinas) UnicastRemoteObject.exportObject(server, 0);

            // Bind the remote object's stub in the registry
            Registry registry = LocateRegistry.getRegistry();
            registry.rebind("BancoDisciplinas", stub);

            System.err.println("Server ready");
        } catch (Exception e) {
            System.err.println("Server exception: " + e.toString());
            e.printStackTrace();
        }
    }
}
