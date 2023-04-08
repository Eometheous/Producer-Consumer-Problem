//
//  main.c
//  Producer Consumer Problem
//
//  Created by Jonathan Thomas on 4/8/23.
//

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <dispatch/dispatch.h>
#include <unistd.h>

const int BUFFER_SIZE = 5;

void *producer(void *params);
void *consumer(void *params);

void add_item(int item);
void remove_item_at(int index);

void print_buffer(int buffer[]);

pthread_mutex_t mutex;
int buffer[BUFFER_SIZE];
int insertPointer = 0;
int removePointer = 0;

dispatch_semaphore_t empty;
dispatch_semaphore_t full;

int main(int argc, const char * argv[]) {
    srand(DISPATCH_TIME_NOW); // set the seed for rand()
    pthread_t producers[atoi(argv[2])];
    pthread_t consumers[atoi(argv[3])];
    
    pthread_mutex_init(&mutex, NULL);
    
    empty = dispatch_semaphore_create(BUFFER_SIZE);
    full = dispatch_semaphore_create(0);
    
    // create producer threads
    printf("Creating Producer Threads\n");
    for (int i = 0; i < atoi(argv[2]); i++) {
        pthread_create(&producers[i], 0, *producer, NULL);
    }
    
    // create consumer threads
    printf("Creating Consumer Threads\n");
    for (int i = 0; i < atoi(argv[3]); i++) {
        pthread_create(&consumers[i], 0, *consumer, NULL);
    }
    
    sleep(atoi(argv[1]));
    printf("\nTimer Finished\nStopping All Threads\n\n");
    
    // stop producer threads
    for (int i = 0; i < atoi(argv[2]); i++) {
        pthread_cancel(producers[i]);
    }
    
    // stop consumer threads
    for (int i = 0; i < atoi(argv[3]); i++) {
        pthread_cancel(consumers[i]);
    }
    return 0;
}

void *producer(void *params) {
    while (TRUE) {
        sleep(rand() % 5);        // sleep for a random amount of time
        int item = rand() % 100 + 1; // random number between 1 - 100
        
        add_item(item);
        printf("Producer added %d\n\n", item);
    }
}

void *consumer(void *params) {
    while (TRUE) {
        sleep(rand() % 5);        // sleep for a random amount of time
        int item_to_be_removed = buffer[removePointer % BUFFER_SIZE];
        remove_item_at(removePointer % BUFFER_SIZE);
        printf("Consumer removed %d\n\n", item_to_be_removed);
    }
}

void add_item(int item) {
    dispatch_semaphore_wait(empty, DISPATCH_TIME_FOREVER);
    pthread_mutex_lock(&mutex);
    
    
    buffer[insertPointer % BUFFER_SIZE] = item;
    insertPointer++;
    print_buffer(buffer);
    
    pthread_mutex_unlock(&mutex);
    dispatch_semaphore_signal(full);
}

void remove_item_at(int index) {
    dispatch_semaphore_wait(full, DISPATCH_TIME_FOREVER);
    pthread_mutex_lock(&mutex);
    
    
    buffer[index] = 0;
    removePointer++;
    print_buffer(buffer);
    
    pthread_mutex_unlock(&mutex);
    dispatch_semaphore_signal(empty);
}

void print_buffer(int buffer[]) {
    for (int i = 0; i < BUFFER_SIZE; i++) {
        printf("%d ", buffer[i]);
    }
    printf("\n");
}
