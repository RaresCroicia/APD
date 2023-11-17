package multipleProducersMultipleConsumers;

import java.util.concurrent.ArrayBlockingQueue;

public class Buffer {
	int value;
	ArrayBlockingQueue<Integer> queue = new ArrayBlockingQueue<Integer>(1);

	void put(int value) {
		try {
			queue.put(value);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
	}

	int get() {
		try {
			return queue.take();
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
		return -1;
	}
}
