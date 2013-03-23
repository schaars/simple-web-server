#ifndef _SHARED_QUEUE_H_
#define _SHARED_QUEUE_H_

#include <pthread.h>

/*
 * A shared queue is a synchronized queue of int which
 * represent the connections socket.
 */
struct shared_queue {
   int size;
   int *queue;
   int read_idx;
   int write_idx;
   int nb_msg;
   pthread_mutex_t mutex;
   pthread_cond_t cond_can_write;
   pthread_cond_t cond_can_read;
};

/*
 * Create and return a new shared_queue of size size.
 */
struct shared_queue* create_shared_queue(const int size);

/*
 * Delete the shared_queue sq and free the associated memory
 */
void delete_shared_queue(struct shared_queue *sq);

/*
 * Add the element e at the tail of the queue sq.
 * Blocks until new room become available.
 */
void shared_queue_add(const int e, struct shared_queue *sq);

/*
 * Get the element at the head of the queue sq
 * Blocks until a new element becomes available.
 */
int shared_queue_get(struct shared_queue *sq);

#endif
