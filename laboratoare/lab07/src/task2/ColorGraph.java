package task2;

import java.util.ArrayList;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.atomic.AtomicInteger;

import static task2.Main.COLORS;
import static task2.Main.N;
import static task2.Main.graph;

public class ColorGraph implements Runnable {

    private final ExecutorService executorService;
    private final int[] colors;
    private final int step;

    public ColorGraph(ExecutorService executorService, int[] colors, Integer step) {
        this.executorService = executorService;
        this.colors = colors;
        this.step = step;
    }

    private boolean verifyColors(int[] colors, int step) {
        for (int i = 0; i < step; i++) {
            if (colors[i] == colors[step] && isEdge(i, step))
                return false;
        }
        return true;
    }

    private boolean isEdge(int a, int b) {
        for (int[] ints : graph) {
            if (ints[0] == a && ints[1] == b)
                return true;
        }
        return false;
    }

    private void printColors(int[] colors) {
        StringBuilder aux = new StringBuilder();
        for (int color : colors) {
            aux.append(color).append(" ");
        }
        System.out.println(aux);
    }

    @Override
    public void run() {
        if (step == N) {
            printColors(colors);
            executorService.shutdown();
        }
        for (int i = 0; i < COLORS; i++) {
            int[] newColors = colors.clone();
            newColors[step] =  i;

            if (verifyColors(newColors, step)) {
                executorService.submit(new ColorGraph(executorService, newColors, step + 1));
            }
        }
    }
}
