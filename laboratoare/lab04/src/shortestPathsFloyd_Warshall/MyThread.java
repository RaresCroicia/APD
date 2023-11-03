package shortestPathsFloyd_Warshall;

public class MyThread extends Thread {
    int id;

    public MyThread(int id) {
        this.id = id;
    }

    public void run() {
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 5; j++) {
                synchronized (MyThread.class) {
                    MainParalel.graph[i][j] = Math.min(MainParalel.graph[i][id] + MainParalel.graph[id][j], MainParalel.graph[i][j]);
                }
            }
        }
    }

}
