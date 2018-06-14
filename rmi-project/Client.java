import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;
import java.util.Scanner;

public class Client {

    private Client() {}

    public static void main(String[] args) {

        String host = (args.length < 1) ? null : args[0];
        try {
            Registry registry = LocateRegistry.getRegistry(host);
            Hello stub = (Hello) registry.lookup("Hello");
        	Scanner reader = new Scanner(System.in);  // Reading from System.in
        	System.out.println("Quantas vezes teremos que saudar o Marcio hoje?");
        	int n = reader.nextInt(); // Scans the next token of the input as an int.
        	//once finished
        	reader.close();
            String response = stub.saudarMarcio(n);
            System.out.println("response: " + response);
        } catch (Exception e) {
            System.err.println("Client exception: " + e.toString());
            e.printStackTrace();
        }
    }
}
