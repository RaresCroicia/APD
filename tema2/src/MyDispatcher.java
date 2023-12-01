/* Implement this class. */

import java.util.List;

public class MyDispatcher extends Dispatcher {

    /**
     * How many tasks have been assigned so far for RoundRobin algorithm.
     */
    private int lastRoundRobinTask;
    /**
     * How many hosts are started.
     */
    private final int numberOfHosts;

    private final int SHORT_NODE_INDEX = 0;
    private final int MEDIUM_NODE_INDEX = 1;
    private final int LONG_NODE_INDEX = 2;

    /**
     * Constructor for MyDispatcher.
     * @param algorithm The scheduling algorithm to use.
     * @param hosts The list of hosts to assign tasks to.
     */
    public MyDispatcher(SchedulingAlgorithm algorithm, List<Host> hosts) {
        super(algorithm, hosts);
        lastRoundRobinTask = -1;
        numberOfHosts = hosts.size();
    }

    /**
     * Add a task to the host using Round Robin Method.
     * @param task The task to add.
     */
    private void addTaskRoundRobin(Task task) {
        int hostIndex = (++lastRoundRobinTask) % numberOfHosts;
        hosts.get(hostIndex).addTask(task);
    }

    /**
     * Add a task to the host using Shortest Queue Method.
     * @param task The task to add.
     */
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

    /**
     * Add a task to the host using Size Interval Task Assignment Method.
     * @param task The task to add.
     */
    private void addTaskSITA(Task task) {
        switch(task.getType()) {
            case SHORT:
                hosts.get(SHORT_NODE_INDEX).addTask(task);
                break;
            case MEDIUM:
                hosts.get(MEDIUM_NODE_INDEX).addTask(task);
                break;
            case LONG:
                hosts.get(LONG_NODE_INDEX).addTask(task);
                break;
            default:
                break;
        }
    }

    /**
     * Add a task to the host using Least Work Left Method.
     * @param task The task to add.
     */
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

    /**
     * Add a task to the host using the chosen scheduling algorithm.
     * @param task The task to add.
     */
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
