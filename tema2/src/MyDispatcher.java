/* Implement this class. */

import java.util.List;

public class MyDispatcher extends Dispatcher {

    int lastRoundRobinTask;
    int numberOfHosts;
    public MyDispatcher(SchedulingAlgorithm algorithm, List<Host> hosts) {
        super(algorithm, hosts);
        lastRoundRobinTask = -1;
        numberOfHosts = hosts.size();
    }

    private void addTaskRoundRobin(Task task) {
        int hostIndex = (lastRoundRobinTask + 1) % numberOfHosts;
        lastRoundRobinTask ++;
        hosts.get(hostIndex).addTask(task);
    }

    private void addTaskShortestQueue(Task task) {
        int minQueueSize = Integer.MAX_VALUE;
        int minQueueIndex = -1;
        for (int i = 0; i < numberOfHosts; i++) {
            int queueSize = hosts.get(i).getQueueSize();
            if (queueSize < minQueueSize) {
                minQueueSize = queueSize;
                minQueueIndex = i;
            }
        }
        hosts.get(minQueueIndex).addTask(task);
    }

    private void addTaskSITA(Task task) {
        switch(task.getType()) {
            case SHORT:
                hosts.get(0).addTask(task);
                break;
            case MEDIUM:
                hosts.get(1).addTask(task);
                break;
            case LONG:
                hosts.get(2).addTask(task);
                break;
            default:
                break;
        }
    }

    private void addTaskLWL(Task task) {
        long minWorkLeft = Integer.MAX_VALUE;
        int minWorkLeftIndex = -1;
        for (int i = 0; i < numberOfHosts; i++) {
            long workLeft = hosts.get(i).getWorkLeft();
            if (workLeft < minWorkLeft) {
                minWorkLeft = workLeft;
                minWorkLeftIndex = i;
            }
        }
        hosts.get(minWorkLeftIndex).addTask(task);
    }
    @Override
    public void addTask(Task task) {
        switch(algorithm) {
            case ROUND_ROBIN:
                addTaskRoundRobin(task);
                break;
            case SHORTEST_QUEUE:
                addTaskShortestQueue(task);
                break;
            case SIZE_INTERVAL_TASK_ASSIGNMENT:
                addTaskSITA(task);
                break;
            case LEAST_WORK_LEFT:
                addTaskLWL(task);
                break;
            default:
                break;
        }
    }
}
