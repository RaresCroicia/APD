package doubleVectorElements;

public class MainParalel {
    public static int[] v = new int[100000013];
    public static void main(String[] args) {
        int N = 100000013;
        int P = 4; // the program should work for any P <= N

        for (int i = 0; i < N; i++) {
            v[i] = i;
        }

        // Parallelize me using P threads
        MyThread[] threads = new MyThread[P];
        for (int i = 0; i < P; i++) {
            threads[i] = new MyThread(i, N, P);
            threads[i].start();
        }

        for (int i = 0; i < P; i++) {
            try {
                threads[i].join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }

        for (int i = 0; i < N; i++) {
            if (v[i] != i * 2) {
                System.out.println("Wrong answer");
                System.exit(1);
            }
        }
        System.out.println("Correct");
    }
}
