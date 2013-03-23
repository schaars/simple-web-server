#ifndef _POOL_H_
#define _POOL_H_

#include "shared_queue.h"

/*
 * Create the pool of pool_size threads and launch the threads.
 * These threads will call the file_manager to deliver files
 * and listen to clients requests from the shared_queue *shq.
 */
void create_pool(const int pool_size, struct shared_queue *shq);

/*
 * Delete the pool of threads and free the associated memory
 */
void delete_pool(void);

#endif
