package task6;


import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.RecursiveTask;


public class QueensProblem extends RecursiveTask<Void> {
    private final int[] graph;
    private final int step;

    public QueensProblem(int[] graph, Integer step) {
        this.graph = graph;
        this.step = step;
    }

    private static boolean check(int[] arr, int step) {
        for (int i = 0; i <= step; i++) {
            for (int j = i + 1; j <= step; j++) {
                if (arr[i] == arr[j] || arr[i] + i == arr[j] + j || arr[i] + j == arr[j] + i)
                    return false;
            }
        }
        return true;
    }

    private static void printQueens(int[] sol) {
        StringBuilder aux = new StringBuilder();
        for (int i = 0; i < sol.length; i++) {
            aux.append("(").append(sol[i] + 1).append(", ").append(i + 1).append("), ");
        }
        aux = new StringBuilder(aux.substring(0, aux.length() - 2));
        System.out.println("[" + aux + "]");
    }

    @Override
    public Void compute() {
        if (task3.Main.N == step) {
            printQueens(graph);
            return null;
        }

        List<QueensProblem> tasks = new ArrayList<>();
        for (int i = 0; i < Main.N; ++i) {
            int[] newGraph = graph.clone();
            newGraph[step] = i;

            if (check(newGraph, step)) {
                QueensProblem task = new QueensProblem(newGraph, step + 1);
                tasks.add(task);
                task.fork();
            }
        }

        for(QueensProblem task : tasks) {
            task.join();
        }

        return null;
    }
}
