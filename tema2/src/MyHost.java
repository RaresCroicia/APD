/* Implement this class. */

import java.util.*;
import java.util.concurrent.PriorityBlockingQueue;

public class MyHost extends Host {
    /**
     * The list of tasks to be executed.
     * The tasks are sorted by their priority and in case of a tie, by their start time.
     */
    private PriorityQueue<Task> tasks;
    /**
     * The task that is currently running.
     */
    private Task runningTask = null;
    /**
     * Whether the host is running.
     */
    private boolean isRunning = true;
    @Override
    public void run() {
        while(isRunning) {
            if(runningTask == null) { // if no task is running try to run one
                if(tasks != null && !tasks.isEmpty()) {
                    Task taskToRun = tasks.poll();
                    if(taskToRun != null) {
                        runningTask = taskToRun;
                    }
                }
            } else if(runningTask.isPreemptible()){ // if a task is running and is preemptible, check if there is a task with higher priority
                if(tasks != null && !tasks.isEmpty()) {
                    Task nextTask = tasks.peek();
                    if(nextTask.getPriority() > runningTask.getPriority()) {
                        tasks.poll();
                        tasks.add(runningTask);
                        runningTask = nextTask;
                    }
                }
            }
            long SLEEP_TIME = 100;
            if(runningTask != null) // if a task is running, run it
                if(runningTask.getLeft() >= SLEEP_TIME) { // if the task is not finished, run it for SLEEP_TIME
                    runningTask.setLeft(runningTask.getLeft() - SLEEP_TIME);
                } else { // if the task is finished, finish it
                    runningTask.finish();
                    runningTask = null;
                }
            try { // Here is basically the actual running of the task
                sleep(SLEEP_TIME);
            } catch (InterruptedException e) {
                throw new RuntimeException(e);
            }
        }
    }

    /**
     * Add a task to the host.
     * @param task task to be added at this host
     */
    @Override
    public void addTask(Task task) {
        if (tasks == null) { // if the tasks queue is not initialized, initialize it
            tasks = new PriorityQueue<>((o1, o2) -> {
                if(o1.getPriority() == o2.getPriority()) {
                    return (o1.getStart() - o2.getStart());
                }
                return (o2.getPriority() - o1.getPriority());
            });
        }
        tasks.add(task);
    }
    /**
     * Get the number of tasks in the queue.
     * @return the number of tasks in the queue + the number of tasks that are currently running
     */
    @Override
    public int getQueueSize() {
        return tasks == null ? 0 : tasks.size() + (runningTask == null ? 0 : 1);
    }

    /**
     * Get the total work left to be done.
     * @return the total work left to be done by the tasks in the queue + by the tasks that are currently running
     */
    @Override
    public long getWorkLeft() {
        long workLeft = 0;
        List <Task> tasksTemp = new ArrayList<>();
        if(this.tasks == null) {
            return 0;
        }
        while(!tasks.isEmpty()) {
            workLeft += tasks.peek().getLeft();
            tasksTemp.add(tasks.poll());
        }
        tasks.addAll(tasksTemp);
        return workLeft + (runningTask == null ? 0 : runningTask.getLeft());
    }

    /**
     * Shutdown the host.
     * Finish all the tasks in the queue and the task that is currently running.
     */
    @Override
    public void shutdown() {
        while(!tasks.isEmpty()) {
            tasks.poll().finish();
        }
        if(runningTask != null) {
            runningTask.finish();
        }
        isRunning = false;
    }
}
