/* Implement this class. */

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

public class MyHost extends Host {
    private ArrayList<Task> tasks;
    private HashMap<Integer, Long> workTimeLeft;
    private boolean isRunning = true;
    @Override
    public void run() {
        while(isRunning) {
            try {
                sleep(1);
            } catch (InterruptedException e) {
                throw new RuntimeException(e);
            }
            // TODO: implementeaza logica de rulare cu preemtie si prioritati si trecere a timpului
        }
    }

    @Override
    public void addTask(Task task) {
        // If tasks is not initialized, initialize it
        if (tasks == null) {
            tasks = new ArrayList<>();
        }
        tasks.add(task);
        if (workTimeLeft == null) {
            workTimeLeft = new HashMap<>();
        }
        workTimeLeft.put(task.getId(), task.getLeft());
    }

    @Override
    public int getQueueSize() {
        return tasks == null ? 0 : tasks.size();
    }

    @Override
    public long getWorkLeft() {
        long workLeft = 0;
        for (Task task : tasks) {
            workLeft += workTimeLeft.get(task.getId());
        }
        return workLeft;
    }

    @Override
    public void shutdown() {
        for (Task task : tasks) {
            if(task.getFinish() == 0)
                task.finish();
        }
        isRunning = false;
    }
}
