package exercitiul1;

public class MyThread extends Thread{
    private int id;

    public MyThread(int id) {
        this.id = id;
    }

    @Override
    public void run() {
        System.out.println("Hello from thread # " + id);
    }

}
