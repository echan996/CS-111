#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <pthread.h>
#include <sys/resource.h>

long long counter = 0;
void add(long long* pointer, long long value) {
	long long sum = *pointer + value;
	*pointer = sum;
}
void* thread_action(void* arg){
	long long iterations = *(long long*)arg;
	for (int i = 0; i < iterations;i++)
		add(&counter, 1);	
	for (int i = 0; i < iterations;i++)
		add(&counter, -1);
}


struct timespec timer;
static struct option long_options[] = {
		{ "threads", required_argument, 0, 'a' },
		{ "iter", required_argument, 0, 'b' },
		{ "iterations", required_argument, 0, 'b' },
		{0,0,0,0}
};


int main(int argc, char** argv){
	printf("suck my dick");
	int threads, iterations;
	threads = iterations = 1;
	int i = 0;
	long operations;
	double per_op;
	long long time_init, time_finish;
	char option;
	while ((option = (char)getopt_long(argc, argv, "", long_options, &i)) != -1){
		switch (option){
		case 'a':
			if ((threads = atoi(optarg)) == 0){
				fprintf(stderr, "Argument must be positive integer\n");
				
			}
			break;
		case 'b':
			if ((iterations = atoi(optarg)) == 0){
				fprintf(stderr, "Argument must be positive integer\n");
			}
			break;

		default:
			fprintf(stderr, "Error: Invalid argument\n");
			break;
		}
	}
	pthread_t *tids = malloc(threads*sizeof(pthread_t));
	if (tids == NULL){
		fprintf(stderr, "Error: memory not allocated\n");
		exit(1);
	}
	clock_gettime(CLOCK_MONOTONIC, &timer);
	time_init = timer.tv_sec * 1000000000 + timer.tv_nsec;
	for (int a = 0, create_check = 0; a < threads; a++){
		create_check = pthread_create(tids + a, 0, thread_action, &iterations);
		if (create_check){
			fprintf(stderr, "Error: unable to create thread\n");
		}
	}
	for (int a = 0; a < threads; a++){
		pthread_join(tids[i], 0);
	}
	
	
	clock_gettime(CLOCK_MONOTONIC, &timer);
	free(tids);
	time_finish = timer.tv_sec * 1000000000 + timer.tv_nsec - time_init;
	operations = threads* iterations * 2;
	fprintf(stdout, "%d threads x %d iterations x (add + subtract) = %ld operations\n", threads, iterations, operations);
	if (counter != 0){
		fprintf(stderr, "Error: final count = %lld\n", counter);
	}
	fprintf(stdout, "elapsed time: %lld\n", time_finish);
	per_op = time_finish / operations;
	fprintf(stdout, "per operation: %f", per_op);
	return 0;
}