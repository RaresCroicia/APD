package readersWriters.conditionedSynchronization;

import java.util.concurrent.Semaphore;

public class ReaderWriterSharedVars {
    // The value to read/write
    volatile int shared_value;
    // TODO: Add semaphores and anything else needed for synchronization
    // Semaphore s;
    int readers;
    int writers;
    int waiting_writers;
    int waiting_readers;
    Semaphore sem_writer;
    Semaphore sem_reader;
    Semaphore enter;


    ReaderWriterSharedVars(int init_shared_value) {
        this.shared_value = init_shared_value;
        //this.s = new Semaphore(1);
    }

}
