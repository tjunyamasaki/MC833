import java.rmi.Remote;
import java.rmi.RemoteException;
import java.sql.SQLException;

public interface BancoDisciplinas extends Remote {
    // Aluno
    String list_codes() throws RemoteException, SQLException;
    String get_ementa(String search_code) throws RemoteException, SQLException;
    String get_comment(String search_code) throws RemoteException, SQLException;
    String get_full_info(String search_code) throws RemoteException, SQLException;
    String get_all_info() throws RemoteException, SQLException;
    // Professor
    int write_comment(String search_code, String comment) throws RemoteException, SQLException;
}
