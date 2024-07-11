// SPDX-License-Identifier: BSD-3-Clause

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>

#include "os_graph.h"
#include "os_threadpool.h"
#include "log/log.h"
#include "utils.h"

#define NUM_THREADS		4

static int sum;
static os_graph_t *graph;
static os_threadpool_t *tp;

/* TODO: Define graph synchronization mechanisms. */
pthread_mutex_t sum_lock;
pthread_mutex_t graph_mutex;

/* TODO: Define graph task argument. */

void process_node_aux(void *data);
static void process_node(unsigned int idx);

void process_node_aux(void *data)
{
	unsigned int arg = *(unsigned int *)data;

	process_node(arg);
}

static void process_node(unsigned int idx)
{
	/* TODO: Implement thread-pool based processing of graph. */
	os_node_t *node = graph->nodes[idx];

	pthread_mutex_lock(&sum_lock);
	sum += node->info;
	graph->visited[node->id] = DONE;
	pthread_mutex_unlock(&sum_lock);

	if (idx == 0 && node->num_neighbours == 0) {
		tp->status = FINISHED;
		pthread_cond_broadcast(&tp->queue_not_empty);
	}

	for (unsigned int i = 0; i < node->num_neighbours; i++) {
		pthread_mutex_lock(&graph_mutex);
		if (graph->visited[node->neighbours[i]] == NOT_VISITED) {
			graph->visited[node->neighbours[i]] = PROCESSING;
			pthread_mutex_unlock(&graph_mutex);

			os_task_t *task = create_task(&process_node_aux, &node->neighbours[i], NULL);

			enqueue_task(tp, task);
		} else {
			pthread_mutex_unlock(&graph_mutex);
		}
	}
}

int main(int argc, char *argv[])
{
	FILE *input_file;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s input_file\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	input_file = fopen(argv[1], "r");
	DIE(input_file == NULL, "fopen");

	graph = create_graph_from_file(input_file);

	/* TODO: Initialize graph synchronization mechanisms. */
	tp = create_threadpool(NUM_THREADS);

	/**************Codul meu incepe aici******************/
	pthread_mutex_init(&sum_lock, NULL);
	pthread_mutex_init(&graph_mutex, NULL);

	/*************Codul meu se termina aici***************/

	process_node(0);
	wait_for_completion(tp);
	destroy_threadpool(tp);

	/*****/
	pthread_mutex_destroy(&sum_lock);
	pthread_mutex_destroy(&graph_mutex);
	/*****/

	printf("%d", sum);

	return 0;
}
