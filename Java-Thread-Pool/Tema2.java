import java.io.*;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Semaphore;

public class Tema2 {
    static String folderInput;
    static int nThreads;
    static String orderInputFile;
    static String orderOutputFile;

    static String productInputFile;
    static String productOutputFile;
    public static FileWriter productOutput;
    public static FileWriter orderOutput;
    public static Semaphore prodSem;
    public static Semaphore ordSem;

    public static void main(String[] args) throws IOException {
        folderInput = args[0];
        nThreads = Integer.parseInt(args[1]);

        orderInputFile = folderInput + "/orders.txt";
        orderOutputFile = "orders_out.txt";
        productInputFile = folderInput + "/order_products.txt";
        productOutputFile = "order_products_out.txt";

        prodSem = new Semaphore(1);
        ordSem = new Semaphore(1);

        File ordersFile = new File(orderOutputFile);
        if (ordersFile.exists()) {
            ordersFile.delete();
        }
        try {
            ordersFile.createNewFile();
        } catch (IOException e) {
            e.printStackTrace();
        }

        File productFile = new File(productOutputFile);
        if (productFile.exists()) {
            productFile.delete();
        }
        try {
            productFile.createNewFile();
        } catch (IOException e) {
            e.printStackTrace();
        }

        productOutput = new FileWriter(productOutputFile);
        orderOutput = new FileWriter(orderOutputFile);

        ExecutorService tpe = Executors.newFixedThreadPool(nThreads);
        int size = (int) Files.size(Paths.get(orderInputFile));

        int toRead = (int) (double) size / nThreads;
        int startPoint;
        int endPoint;

        Thread[] orderThreads = new Thread[nThreads];
        for (int i = 0; i < nThreads; i++) {
            startPoint = (int) (i * (double) size / nThreads);
            endPoint = (int) ((i + 1) * (double) size / nThreads);
            orderThreads[i] = new Thread(new OrderThread(tpe, startPoint, endPoint));
            orderThreads[i].start();
        }

        for (int i = 0; i < nThreads; i++) {
            try {
                orderThreads[i].join();
            } catch (InterruptedException e) {
                throw new RuntimeException(e);
            }
        }

        productOutput.close();
        orderOutput.close();
        tpe.shutdown();
    }
}