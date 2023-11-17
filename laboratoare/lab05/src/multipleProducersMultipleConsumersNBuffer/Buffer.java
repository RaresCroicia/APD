package multipleProducersMultipleConsumersNBuffer;

import java.util.Queue;
import java.util.concurrent.Semaphore;

import static multipleProducersMultipleConsumersNBuffer.Main.BUFFER_SIZE;

public class Buffer {
    
    Queue<Integer> queue;
    Semaphore gol = new Semaphore(BUFFER_SIZE);
    Semaphore plin = new Semaphore(0);
    Semaphore mutex = new Semaphore(1);
    
    public Buffer(int size) {
        queue = new LimitedQueue<>(size);
    }

	public void put(int value) {
        try {
            gol.acquire();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        try {
            mutex.acquire();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        queue.add(value);
        mutex.release();
        plin.release();
	}

	public int get() {
        int a = -1;
        try {
            plin.acquire();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        try {
            mutex.acquire();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        Integer result = queue.poll();
        if (result != null) {
            a = result;
        }
        mutex.release();
        gol.release();
        return a;
	}
}
