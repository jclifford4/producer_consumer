/*
 * The code is not part of the real application, and just used to 
 * illustrate the bounded-buffer problem using Semaphore and/or mutexes. 
 * Detailed requirements please refer to the assignment documentation.
 * 
*/

#include <stdio.h> 
#include <stdlib.h> 
#include "bbuffer.h"
#include <semaphore.h>  // sem_wait(), sem_post()
#include <pthread.h>


// You may need to define more variables here
buffer_t buffer;
pthread_t consumer_tid[CONSUMERS];
pthread_t producer_tid[PRODUCERS];
sem_t empty;
sem_t full;
sem_t mutex;


void initilization()
{
    /* TODO: intialize golbal variables if you need, such as semaphore, mutex and etc. Leave it empyty if you
     * do not need it.
     */     
    sem_init(&empty, 0, BUFFER_SIZE);   // All are empty to begin
    sem_init(&full, 0, 0);              // 0 are full
    sem_init(&mutex, 0, 1);             // is a lock starts at 1

     
}

/* *
 * insert_item - thread safe function to insert items to the bounded buffer
 * @param item the value to be inserted
 * @return 0 in case of sucess -1 otherwise
 */
  
int insert_item(int item, long int id)
{
    /* TODO: Check and wait if the buffer is full. Ensure exclusive
     * access to the buffer and may use the existing code to insert an item.
     */   
     
    sem_wait(&empty);       // signals empty slots
    sem_wait(&mutex);       // locks shared data
    buffer.value[buffer.next_in] = item;
    
      
    printf("producer %ld: inserted item %d into buffer index %d\n", id, item, buffer.next_in);
       
    buffer.next_in = (buffer.next_in + 1) % BUFFER_SIZE;
    sem_post(&mutex);       // unlocks shared data
    sem_post(&full);        // signals full slots
        
    return 0;
}

/**
 * remove_item - thread safe function to remove items to the bounded buffer
 * @param item the address of the variable that the removed value will be written
 * @return 0 in case of sucess -1 otherwise
 */
int remove_item(int *item, long int id)
{
    /* TODO: Check and wait if the buffer is empty. Ensure exclusive
     * access to the buffer and use the existing code to remove an item.
     */


    
    sem_wait(&full);        // signals full slots
    sem_wait(&mutex);       // locks shared data
    *item = buffer.value[buffer.next_out];
    buffer.value[buffer.next_out] = -1;    
    
   
    printf("consumer %ld: removed item %d from buffer index %d\n", id, *item, buffer.next_out);
       
    buffer.next_out = (buffer.next_out + 1) % BUFFER_SIZE;
    sem_post(&mutex);       // unlocks shared data
    sem_post(&empty);       // signals empty
    
    return 0;
}

/**
 * producer - will iterate PRODUCER_ITERATION times and call the corresponding
 * function to insert an integer to the bounded buffer
 * @param param an integer id of the producer used to distinguish between the
 * multiple producer threads
 * @return nothing
 */
void * producer(void *param)
{
    int item, i;
    long int id = (long int)param;

    printf("producer %d started!\n", id);
    i = PRODUCER_ITERATIONS;
    while (i--) {
	  sleep(rand() % 3);

	item = rand() % 10000;
	if (insert_item(item, id))
	    printf("Error while inserting to buffer\n");
    }

    pthread_exit(0);
}

/**
 * consumer - will iterate CONSUMER_ITERATION times and call the corresponding
 * function to remove an integer from the bounded buffer
 * @param param an integer id of the producer used to distinguish between the
 * multiple consumer threads
 * @return nothing
 */
void * consumer(void *param)
{
    int item, i;
    long int id = (long int)param;

    printf("consumer %d started!\n", id);
    i = CONSUMER_ITERATIONS;
    while (i--) {
	sleep(rand() % 3);

	if (remove_item(&item, id))
	    printf("Error while removing from buffer\n");
    }

    pthread_exit(0);
}
