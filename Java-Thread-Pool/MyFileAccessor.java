import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.RandomAccessFile;

public class MyFileAccessor {
    RandomAccessFile file;

    public MyFileAccessor(String filePath) throws FileNotFoundException {
        file = new RandomAccessFile(filePath, "rw");
    }

    public byte[] readBytesFromFile(int position, int size)
            throws IOException {
        file.seek(position);
        byte[] bytes = new byte[size];
        file.read(bytes);
        return bytes;
    }

    public byte readByteFromFile(long position) throws IOException {
        file.seek(position);
        return file.readByte();
    }

    public void writeToFile(String data, int position)
            throws IOException {
        file.seek(position);
        file.write(data.getBytes());
        file.close();
    }

    public String readLineFromFile() throws IOException {
        return file.readLine();
    }

    public void seek(long position) throws IOException {
        file.seek(position);
    }
    
    public void close() throws IOException {
        file.close();
    }

    public long getOffset() throws IOException {
        return file.getFilePointer();
    }
}
