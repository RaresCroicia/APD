package barber;

public class Barber extends Thread {
    @Override
    public void run() {
        int servedClients = 0;

        do {
            // TODO
            try {
                Main.client.acquire();
                Main.chairMutex.acquire();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }

            Main.chairs++;

            Main.barber.release();
            Main.chairMutex.release();

            System.out.println("Barber served client");
            ++servedClients;

        } while (checkIfThereAnyClients());
    }

    private boolean checkIfThereAnyClients() {
        int count = 0;
        for (var client: Main.leftClients) {
            if (client == 0) {
                count++;
            }
        }

        return count != 0;
    }
}
