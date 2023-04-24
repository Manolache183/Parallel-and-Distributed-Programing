import java.io.*;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Semaphore;

public class OrderThread implements Runnable {
    ExecutorService tpe;
    private final MyFileAccessor input;
    private final long prevOffset;
    private final int endPoint;

    public OrderThread(ExecutorService tpe, long prevOffset, int endPoint) throws FileNotFoundException {
        this.tpe = tpe;
        this.prevOffset = prevOffset;
        this.endPoint = endPoint;

        input = new MyFileAccessor(Tema2.orderInputFile);
    }

    @Override
    public void run() {
        long lineOffset;

        try {
            lineOffset = getOffset(prevOffset);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }

        String line = "";
        int maxProduct;

        try {
            input.seek(lineOffset);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }

        while (lineOffset < endPoint) {
            try {
                line = input.readLineFromFile();
            } catch (IOException e) {
                throw new RuntimeException(e);
            }

            if (line == null) {
                break;
            }

            String[] parsed = line.split(",");

            maxProduct = Integer.parseInt(parsed[1]);

            String order = parsed[0];
            Semaphore sem = new Semaphore(-maxProduct + 1);
            try {
                for (int i = 1; i <= maxProduct; i++) {
                    tpe.submit(new ProductThread(i, order, sem));
                }
            } catch (FileNotFoundException e) {
                throw new RuntimeException(e);
            }

            try {
                sem.acquire();
            } catch (InterruptedException e) {
                throw new RuntimeException(e);
            }
            String message = line + ",shipped\n";
            
            if (maxProduct > 0) {
                try {
                    Tema2.ordSem.acquire();
                    Tema2.orderOutput.write(message);
                    Tema2.ordSem.release();
                } catch (Exception e) {
                    throw new RuntimeException(e);
                }
            }

            try {
                lineOffset = input.getOffset();
            } catch (Exception e) {
                throw new RuntimeException(e);
            }
            
        }

        try {
            input.close();
        } catch (IOException e) {
            throw new RuntimeException(e);
        }

    }

    private long getOffset(long localPrevOffset) throws IOException {
        long offset = localPrevOffset;
        byte prevChar = input.readByteFromFile(localPrevOffset);

        if (prevChar == '\n') {
            return (offset + 1);
        }

        if (localPrevOffset != 0) {
            prevChar = input.readByteFromFile(localPrevOffset - 1);
            if (prevChar == '\n') {
                return localPrevOffset;
            } else {
                while (prevChar != '\n') {
                    localPrevOffset++;
                    prevChar = input.readByteFromFile(localPrevOffset);
                }

                return localPrevOffset + 1;
            }
        }

        return offset;
    }
}