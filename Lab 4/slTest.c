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
#include "SortedList.h"

char locker = '\0';
long long counter = 0;
int s_test_lock = 0;
pthread_mutex_t m_test_mutex;
SortedList_t *list;
int opt_yield = 0;

typedef struct s_thread_info{
	int iterations;
	int thread_num;
	SortedListElement_t** key_array;
}thread_info;

void* thread_action(void* arg){
    thread_info t_data= *(thread_info*)arg;
	if (locker == 'm'){
		for (int i = 0; i < t_data.iterations; i++){
			pthread_mutex_lock(&m_test_mutex);
			SortedList_insert(list, &t_data.key_array[t_data.thread_num][i]);
			pthread_mutex_unlock(&m_test_mutex);
		}

		int i = SortedList_length(list);

		for (int i = 0; i < t_data.iterations; i++){
			pthread_mutex_lock(&m_test_mutex);
			SortedList_delete(SortedList_lookup(list, t_data.key_array[t_data.thread_num][i].key));
			pthread_mutex_lock(&m_test_mutex);
		}
	}
	else if (locker == 's'){
		for (int i = 0; i < t_data.iterations; i++){
			while (__sync_lock_test_and_set(&s_test_lock, 1));
			SortedList_insert(list, &t_data.key_array[t_data.thread_num][i]);
			__sync_lock_release(&s_test_lock);
		}

		int i = SortedList_length(list);

		for (int i = 0; i < t_data.iterations; i++){
			while (__sync_lock_test_and_set(&s_test_lock, 1));
			SortedList_delete(SortedList_lookup(list, t_data.key_array[t_data.thread_num][i].key));
			__sync_lock_release(&s_test_lock);
		}
	}
	else{
		for (int i = 0; i < t_data.iterations; i++){
			SortedList_insert(list, &t_data.key_array[t_data.thread_num][i]);
		}

		int i = SortedList_length(list);

		for (int i = 0; i < t_data.iterations; i++){
			SortedList_delete(SortedList_lookup(list, t_data.key_array[t_data.thread_num][i].key));
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
				for (int i = 0; optarg[i] != '\0'; i++){
					if (optarg[i] == 'i')
						opt_yield = opt_yield | INSERT_YIELD;
					else if (optarg[i] == 'd')
						opt_yield = opt_yield | DELETE_YIELD;
					else if (optarg[i] == 's')
						opt_yield = opt_yield | SEARCH_YIELD;
					else{
						fprintf(stderr, "Error: Arguments to --yield must be a member of [ids]\n");
						break;
					}
				}
                break;
            case 'd':
                locker = optarg[0];
				if (locker != 'm' && locker != 's'){
					fprintf(stderr, "Error: invalid sync option\n");
					exit(1);
				}
                break;
                
            default:
                fprintf(stderr, "Error: Invalid argument\n");
                break;
        }
    }
    SortedListElement_t **arr = (SortedListElement_t **)malloc(threads*sizeof(SortedListElement_t *));
    for(int i=0;i<threads;i++){
      arr[i]=(SortedListElement_t*)malloc(iterations*sizeof(SortedListElement_t));
    }
    
    // initialize empty list
    list = (SortedList_t *)malloc(sizeof(SortedList_t));
    list->next = NULL;
    list->prev = NULL;
    list->key = NULL;
    //printf("here!");
    static const char alphanum[] =     "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	for (int t = 0; t < threads; t++){
		for (int i = 0; i < iterations; i++){
			int len = (rand() % 49) + 1; // random length from 1 to 50
			char *s = (char *)malloc(len+1);
			for (int j = 0; j < len; ++j) {
				s[j] = alphanum[rand() % (52)];
			}
			s[len] = '\0';
			arr[t][i].next = NULL;
			arr[t][i].prev = NULL;
			arr[t][i].key = s;
			//fprintf(stdout,"%s\n",arr[t][i].key);
		}
   	}
	//fprintf(stdout,"here!");
	 pthread_t *tids = malloc(threads*sizeof(pthread_t));
    if (tids == NULL){
        fprintf(stderr, "Error: memory not allocated\n");
        exit(1);
    }

	thread_info *thread_data = malloc(threads*sizeof(thread_info));
	
	if (thread_data == NULL){
		fprintf(stderr, "Error: memory not allocated\n");
		exit(1);
	}
	
	for (int i = 0; i < threads; i++){
		thread_data[i].iterations = iterations;
		thread_data[i].thread_num = i;
		thread_data[i].key_array = arr;
		
	}
	
    clock_gettime(CLOCK_MONOTONIC, &timer);
    time_init = timer.tv_sec * 1000000000 + timer.tv_nsec;
    //    fprintf(stdout,"here!");
	for (int a = 0, create_check = 0; a < threads; a++){
        create_check = pthread_create(tids + a, 0, thread_action, &thread_data[a]);
        if (create_check){
            fprintf(stderr, "Error: unable to create thread\n");
        }
    }
    for (int a = 0; a < threads; a++){
        pthread_join(tids[a], 0);
    }
    if(SortedList_length(list))
      fprintf(stdout,"Error: %d length", SortedList_length(list));
     /*
    
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
	*/return 0;
}
