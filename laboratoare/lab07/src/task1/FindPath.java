package task1;

import java.util.ArrayList;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.atomic.AtomicInteger;

import static task1.Main.graph;

public class FindPath implements Runnable {

    private final ExecutorService executorService;
    private final ArrayList<Integer> partialPath;
    private final AtomicInteger inQueue;

    private final int destination;

    public FindPath(ExecutorService executorService, ArrayList<Integer> partialPath, AtomicInteger inQueue, int destination) {
        this.executorService = executorService;
        this.partialPath = partialPath;
        this.inQueue = inQueue;
        this.destination = destination;
    }

    @Override
    public void run() {
        if (partialPath.get(partialPath.size() - 1) == destination) {
            System.out.println(partialPath);
        }

        int lastNodeInPath = partialPath.get(partialPath.size() - 1);
        for (int[] ints : graph) {
            if (ints[0] == lastNodeInPath) {
                if (partialPath.contains(ints[1]))
                    continue;
                ArrayList<Integer> newPartialPath = new ArrayList<>(partialPath);
                inQueue.incrementAndGet();
                newPartialPath.add(ints[1]);
                executorService.submit(new FindPath(executorService, newPartialPath, inQueue, destination));
            }
        }

        int left = inQueue.decrementAndGet();
        if (left == 0) {
            executorService.shutdown();
        }
    }
}
