package exercitiul1;

public class Main {
    private static final int cores = Runtime.getRuntime().availableProcessors();
    private static final MyThread[] threads = new MyThread[cores];

    public static void main(String[] args) {
        for(int i = 0; i < cores; i++) {
            threads[i] = new MyThread(i);
            threads[i].start();
        }

        try {
            for(int i = 0; i < cores; i++) {
                threads[i].join();
            }
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }
}
