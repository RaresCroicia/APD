package philosophersProblem;

import java.util.concurrent.Semaphore;

public class Philosopher implements Runnable {
    private final Object leftFork;
    private final Object rightFork;
    private final int id;

    Semaphore[] locks = new Semaphore[Main.N];
    // initialize all locks to 1

    public Philosopher(int id, Object leftFork, Object rightFork) {
        this.leftFork = leftFork;
        this.rightFork = rightFork;
        this.id = id;
        for(int i = 0; i < Main.N; i++) {
            locks[i] = new Semaphore(1);
        }
    }

    private void sleep() {
        try {
            Thread.sleep(100);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    @Override
    public void run() {
        synchronized (leftFork) {
            if(id != 0) {
                try {
                    locks[id].acquire();
                    locks[(id+1) % Main.N].acquire();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                locks[id].release();
                locks[(id+1) % Main.N].release();
            }
            else {
                try {
                    locks[1].acquire();
                    locks[0].acquire();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                locks[(id+1) % Main.N].release();
                locks[id].release();
            }
            synchronized (rightFork) {
                System.out.println("Philosopher " + id + " is eating");
            }
        }
    }
}
