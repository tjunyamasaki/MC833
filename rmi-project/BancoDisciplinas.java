import java.rmi.Remote;
import java.rmi.RemoteException;

public interface BancoDisciplinas extends Remote {
    // Aluno
    String list_codes() throws RemoteException;
    String get_ementa(String search_code) throws RemoteException;
    String get_comment(String search_code) throws RemoteException;
    String get_full_info(String search_code) throws RemoteException;
    String get_all_info() throws RemoteException;
    // Professor
    int write_comment(String search_code, String comment) throws RemoteException;
}
