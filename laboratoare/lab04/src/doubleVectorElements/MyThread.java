package doubleVectorElements;

public class MyThread extends Thread{
    private int id, N, P;

    public MyThread(int id, int N, int P) {
        this.id = id;
        this.N = N;
        this.P = P;
    }


    public void run() {
        int start = id * (N / P);
        int end = Math.min((id + 1) * (N / P), N);
        for (int i = id; i < N; i += P) {
            MainParalel.v[i] = MainParalel.v[i] * 2;
        }
    }
}
