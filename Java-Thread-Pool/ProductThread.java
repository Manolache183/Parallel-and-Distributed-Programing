import java.io.*;
import java.util.concurrent.Semaphore;

public class ProductThread implements Runnable {
    private int givenProduct;
    private final String order;
    private BufferedReader input;

    private final Semaphore sem;

    public ProductThread(int givenProduct, String order, Semaphore sem) throws FileNotFoundException {
        this.givenProduct = givenProduct;
        this.order = order;
        this.sem = sem;
    }

    @Override
    public void run() {
        String line;
        try {
            input = new BufferedReader(new FileReader(Tema2.productInputFile));
        } catch (IOException e) {
            throw new RuntimeException(e);
        }

        while (true) {
            try {
                line = input.readLine();
            } catch (IOException e) {
                throw new RuntimeException(e);
            }

            if (line == null) {
                System.out.println("N-am gasit produsul coane");
                break;
            }

            if (line.contains(order)) {
                if (givenProduct == 1) {
                    String message = line + ",shipped\n";
                    try {
                        Tema2.prodSem.acquire();
                        Tema2.productOutput.append(message);
                        Tema2.prodSem.release();
                    } catch (Exception e) {
                        throw new RuntimeException(e);
                    }
                    break;
                }
                givenProduct--;
            }
        }

        try {
            input.close();
        } catch (IOException e) {
            throw new RuntimeException(e);
        }

        sem.release();
    }
}