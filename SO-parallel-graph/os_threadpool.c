// SPDX-License-Identifier: BSD-3-Clause

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>

#include "os_threadpool.h"
#include "log/log.h"
#include "utils.h"

/* Create a task that would be executed by a thread. */
os_task_t *create_task(void (*action)(void *), void *arg, void (*destroy_arg)(void *))
{
	os_task_t *t;

	t = malloc(sizeof(*t));
	DIE(t == NULL, "malloc");

	t->action = action;		// the function
	t->argument = arg;		// arguments for the function
	t->destroy_arg = destroy_arg;	// destroy argument function

	return t;
}

/* Destroy task. */
void destroy_task(os_task_t *t)
{
	if (t->destroy_arg != NULL)
		t->destroy_arg(t->argument);
	free(t);
}

/*
 * Check if queue is empty.
 * This function should be called in a synchronized manner.
 */
static int queue_is_empty(os_threadpool_t *tp)
{
	return list_empty(&tp->head);
}

/* Put a new task to threadpool task queue. */
void enqueue_task(os_threadpool_t *tp, os_task_t *t)
{
	assert(tp != NULL);
	assert(t != NULL);

	/* TODO: Enqueue task to the shared task queue. Use synchronization. */

	/**************Codul meu incepe aici******************/

	pthread_mutex_lock(&(tp->queue_lock));
	if (tp->status == NOT_STARTED)
		tp->status = STARTED;
	pthread_cond_signal(&tp->queue_not_empty);
	list_add(&tp->head, &t->list);
	pthread_mutex_unlock(&tp->queue_lock);

	/*************Codul meu se termina aici***************/
}

/*
 * Get a task from threadpool task queue.
 * Block if no task is available.
 * Return NULL if work is complete, i.e. no task will become available,
 * i.e. all threads are going to block.
 */

os_task_t *dequeue_task(os_threadpool_t *tp)
{
	os_task_t *t = NULL;

	/* TODO: Dequeue task from the shared task queue. Use synchronization. */

	/**************Codul meu incepe aici******************/

	pthread_mutex_lock(&(tp->queue_lock));

	if (tp->idle_threads == tp->num_threads - 1 && tp->status == STARTED && queue_is_empty(tp)) {
		tp->status = FINISHED;
		pthread_cond_broadcast(&tp->queue_not_empty);
		pthread_mutex_unlock(&tp->queue_lock);
		return NULL;
	}

	tp->idle_threads++;
	while (queue_is_empty(tp)) {
		pthread_cond_wait(&tp->queue_not_empty, &tp->queue_lock);
		if (tp->status == FINISHED) {
			pthread_mutex_unlock(&tp->queue_lock);
			return NULL;
		}
	}
	tp->idle_threads--;

	if (tp->status == FINISHED) {
		pthread_mutex_unlock(&tp->queue_lock);
		return NULL;
	}

	if (!queue_is_empty(tp)) {
		t = list_entry(tp->head.prev, os_task_t, list);
		list_del(tp->head.prev);
	}
	pthread_mutex_unlock(&tp->queue_lock);

	/*************Codul meu se termina aici***************/
	return t;
}

/* Loop function for threads */
static void *thread_loop_function(void *arg)
{
	os_threadpool_t *tp = (os_threadpool_t *) arg;

	while (1) {
		os_task_t *t;

		if (tp->status == FINISHED)
			break;
		t = dequeue_task(tp);
		if (t == NULL)
			break;
		t->action(t->argument);
		destroy_task(t);
	}
	return NULL;
}

/* Wait completion of all threads. This is to be called by the main thread. */
void wait_for_completion(os_threadpool_t *tp)
{
	/* TODO: Wait for all worker threads. Use synchronization. */

	/* Join all worker threads. */
	for (unsigned int i = 0; i < tp->num_threads; i++)
		pthread_join(tp->threads[i], NULL);
}

/* Create a new threadpool. */
os_threadpool_t *create_threadpool(unsigned int num_threads)
{
	os_threadpool_t *tp = NULL;
	int rc;

	tp = malloc(sizeof(*tp));
	DIE(tp == NULL, "malloc");

	list_init(&tp->head);

	/* TODO: Initialize synchronization data. */

	/**************Codul meu incepe aici******************/

	tp->status = NOT_STARTED;
	tp->idle_threads = 0;

	pthread_mutex_init(&tp->queue_lock, NULL);
	pthread_cond_init(&tp->all_tasks_done, NULL);
	pthread_cond_init(&tp->queue_not_empty, NULL);

	/*************Codul meu se termina aici**************/

	tp->num_threads = num_threads;
	tp->threads = malloc(num_threads * sizeof(*tp->threads));
	DIE(tp->threads == NULL, "malloc");
	for (unsigned int i = 0; i < num_threads; ++i) {
		rc = pthread_create(&tp->threads[i], NULL, &thread_loop_function, (void *) tp);
		DIE(rc < 0, "pthread_create");
	}

	return tp;
}

/* Destroy a threadpool. Assume all threads have been joined. */
void destroy_threadpool(os_threadpool_t *tp)
{
	os_list_node_t *n, *p;

	/* TODO: Cleanup synchronization data. */

	/**************Codul meu incepe aici******************/

	pthread_mutex_destroy(&tp->queue_lock);
	pthread_cond_destroy(&tp->all_tasks_done);
	pthread_cond_destroy(&tp->queue_not_empty);

	/*************Codul meu se termina aici***************/

	list_for_each_safe(n, p, &tp->head) {
		list_del(n);
		destroy_task(list_entry(n, os_task_t, list));
	}

	free(tp->threads);
	free(tp);
}