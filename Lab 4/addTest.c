#define _POSIX_C_SOURCE 199309L
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <pthread.h>
#include <sys/resource.h>
char locker = '\0';
long long counter = 0;
int opt_yield=0;
int s_test_lock = 0;
pthread_mutex_t m_test_mutex;

void add(long long* pointer, long long value) {
	long long sum = *pointer + value;
	if (opt_yield)
		pthread_yield();
	*pointer = sum;
}
void* thread_action(void* arg){
	int iterations = *(int*)arg;
	if (locker == 'm'){
		for (int i = 0; i < iterations; i++){
			pthread_mutex_lock(&m_test_mutex);
			add(&counter, 1);
			add(&counter, -1);
			pthread_mutex_unlock(&m_test_mutex);
		}
	}
	else if(locker=='s'){
		for (int i = 0; i < iterations; i++){
			while (__sync_lock_test_and_set(&s_test_lock,1));
			add(&counter, 1);
			add(&counter, -1);
			__sync_lock_release(&s_test_lock);
		}
	}
	else if (locker == 'c'){
		for (int i = 0; i < iterations; i++){
			do{
				int old = counter;
				if (opt_yield)
					pthread_yield();
				int sum = old + 1;
			}while(__sync_val_compare_and_swap(&counter, old, sum)==*counter);
			do{
				int old = counter;
				if (opt_yield)
					pthread_yield();
				int sum = old - 1;
			} while (__sync_val_compare_and_swap(&counter, old, sum) == *counter);
		}
	}
	else if(locker=='\0'){
		for (int i = 0; i < iterations; i++){
			add(&counter, 1);
			add(&counter, -1);
		}
	}
}


struct timespec timer;
static struct option long_options[] = {
		{ "threads", required_argument, 0, 'a' },
		{ "iter", required_argument, 0, 'b' },
		{ "iterations", required_argument, 0, 'b' },
		{ "yield", required_argument, 0, 'c' },
		{ "sync", required_argument, 0, 'd' },
		{0,0,0,0}
};


int main(int argc, char** argv){
	pthread_mutex_init(&m_test_mutex, NULL);
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
		case 'c':
			if (atoi(optarg) == 1)
				opt_yield = 1;
			break;
		case 'd':
			locker = optarg[0];
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
		pthread_join(tids[a], 0);
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
	fprintf(stdout, "per operation: %f\n", per_op);
	return 0;
}