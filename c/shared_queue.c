#include "shared_queue.h"
#include "util.h"

/*
 * Create and return a new shared_queue of size size.
 */
struct shared_queue* create_shared_queue(const int size) {
   struct shared_queue *sq = malloc(sizeof(*sq));
   if (sq == NULL) {
      die("Malloc error!");
   }

   sq->size = size;
   sq->read_idx = 0;
   sq->write_idx = 0;
   sq->nb_msg = 0;
   pthread_mutex_init(&sq->mutex, NULL);
   pthread_cond_init(&sq->cond_can_write, NULL);
   pthread_cond_init(&sq->cond_can_read, NULL);
   sq->queue = malloc(sizeof(*sq->queue)*sq->size);
   if (!sq->queue) {
      die("Malloc error!");
   }

   return sq;
}

/*
 * Delete the shared_queue sq and free the associated memory
 */
void delete_shared_queue(struct shared_queue *sq) {
   free(sq->queue);
   free(sq);
}

/*
 * Add the element e at the tail of the queue sq.
 * Blocks until new room become available.
 */
void shared_queue_add(const int e, struct shared_queue *sq) {
   pthread_mutex_lock(&sq->mutex);

   while (sq->nb_msg >= sq->size)
   {
      pthread_cond_wait(&sq->cond_can_write, &sq->mutex);
   }

   sq->queue[sq->write_idx] = e;
   sq->write_idx = (sq->write_idx + 1) % sq->size;

   if (++sq->nb_msg == 1)
   {
      pthread_cond_signal(&sq->cond_can_read);
   }

   pthread_mutex_unlock(&sq->mutex);
}

/*
 * Get the element at the head of the queue sq
 * Blocks until a new element becomes available.
 */
int shared_queue_get(struct shared_queue *sq) {
   int e;

   pthread_mutex_lock(&sq->mutex);
   while (sq->nb_msg < 1)
   {
      pthread_cond_wait(&sq->cond_can_read, &sq->mutex);
   }

   e = sq->queue[sq->read_idx];
   sq->read_idx = (sq->read_idx + 1) % sq->size;

   if (sq->nb_msg-- == sq->size)
   {
      pthread_cond_signal(&sq->cond_can_write);
   }

   pthread_mutex_unlock(&sq->mutex);

   return e;
}

