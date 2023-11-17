package oneProducerOneConsumer;

import java.util.concurrent.Semaphore;

public class Buffer {
    private int a;
    Semaphore gol = new Semaphore(1);
    Semaphore full = new Semaphore(0);


    void put(int value) {
        try {
            gol.acquire();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        a = value;
        full.release();
    }

    int get() {
        try {
            full.acquire();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        int value = a;
        gol.release();
        return value;
    }
}
