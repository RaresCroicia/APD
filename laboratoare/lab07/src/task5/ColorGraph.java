package task5;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.RecursiveTask;

import static task5.Main.COLORS;
import static task5.Main.N;
import static task5.Main.graph;


public class ColorGraph extends RecursiveTask<Void> {

    private final int[] colors;
    private final int step;

    public ColorGraph(int[] colors, Integer step) {
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
    protected Void compute() {
        if (step == N) {
            printColors(colors);
            return null;
        }

        List<ColorGraph> tasks = new ArrayList<>();
        for (int i = 0; i < COLORS; i++) {
            int[] newColors = colors.clone();
            newColors[step] =  i;

            if (verifyColors(newColors, step)) {
                ColorGraph task = new ColorGraph(newColors, step + 1);
                tasks.add(task);
                task.fork();
            }
        }

        for(ColorGraph task : tasks) {
            task.join();
        }

        return null;
    }
}
