package shortestPathsFloyd_Warshall;

public class MainParalel {
    static int M = 9;
    static int[][] graph = {{0, 1, M, M, M},
                            {1, 0, 1, M, M},
                            {M, 1, 0, 1, 1},
                            {M, M, 1, 0, M},
                            {M, M, 1, M, 0}};

    public static void main(String[] args) {

        MyThread[] threads = new MyThread[5];

        for(int i = 0; i < 5; i++) {
            threads[i] = new MyThread(i);
            threads[i].start();
        }

        for(int i = 0; i < 5; i++) {
            try {
                threads[i].join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }

        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 5; j++) {
                System.out.print(graph[i][j] + " ");
            }
            System.out.println();
        }
    }
}
