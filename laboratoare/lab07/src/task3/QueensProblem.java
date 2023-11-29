package task3;

import java.util.concurrent.ExecutorService;

public class QueensProblem implements Runnable {

    private final ExecutorService executorService;
    private final int[] graph;
    private final int step;

    public QueensProblem(ExecutorService executorService, int[] graph, Integer step) {
        this.executorService = executorService;
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
    public void run() {
        if (Main.N == step) {
            printQueens(graph);
            executorService.shutdown();
        }
        for (int i = 0; i < Main.N; ++i) {
            int[] newGraph = graph.clone();
            newGraph[step] = i;

            if (check(newGraph, step)) {
                executorService.submit(new QueensProblem(executorService, newGraph, step + 1));
            }
        }
    }
}
