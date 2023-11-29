package task4;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.RecursiveTask;

import static task4.Main.graph;

public class FindPath extends RecursiveTask<Void> {
    private final ArrayList<Integer> partialPath;
    private final int destination;

    public FindPath(ArrayList<Integer> partialPath, int destination) {
        this.partialPath = partialPath;
        this.destination = destination;
    }

    @Override
    protected Void compute() {
        if (partialPath.get(partialPath.size() - 1) == destination) {
            System.out.println(partialPath);
            return null;
        }

        // se verifica nodurile pentru a evita ciclarea in graf
        int lastNodeInPath = partialPath.get(partialPath.size() - 1);
        List<FindPath> tasks = new ArrayList<>();

        for (int[] ints : graph) {
            if (ints[0] == lastNodeInPath) {
                if (partialPath.contains(ints[1]))
                    continue;
                ArrayList<Integer> newPartialPath = new ArrayList<>(partialPath);
                newPartialPath.add(ints[1]);
                FindPath t = new FindPath(newPartialPath, destination);
                tasks.add(t);
                t.fork();
            }
        }

        for (var task: tasks) {
            task.join();
        }

        return null;
    }
}
