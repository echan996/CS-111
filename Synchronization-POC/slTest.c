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
int* s_test_locks;
pthread_mutex_t* m_test_mutexs;
SortedList_t **list;
int opt_yield = 0;

typedef struct s_thread_info{
	int iterations;
	int thread_num;
	SortedListElement_t** key_array;
}thread_info;

int hash_func(char a){
	return ((int)a) % numlists;
}

void* thread_action(void* arg){

    thread_info t_data= *(thread_info*)arg;
	int list_num;
	if (locker == 'm'){
		for (int i = 0; i < t_data.iterations; i++){ 
			
			list_num = hash_func(t_data.key_array[t_data.thread_num][i].key[0]);
			pthread_mutex_lock(&m_test_mutexs[list_num]);
			SortedList_insert(list[list_num], &t_data.key_array[t_data.thread_num][i]);
			pthread_mutex_unlock(&m_test_mutexs[list_num]);
		}

		int i = SortedList_length(list);

		for (int i = 0; i < t_data.iterations; i++){
			list_num = hash_func(t_data.key_array[t_data.thread_num][i].key[0]);
			pthread_mutex_lock(&m_test_mutexs[list_num]);
			
			SortedList_delete(SortedList_lookup(list[list_num], t_data.key_array[t_data.thread_num][i].key));
			pthread_mutex_unlock(&m_test_mutexs[list_num]);
		}
	}
	else if (locker == 's'){
		for (int i = 0; i < t_data.iterations; i++){
			list_num = hash_func(t_data.key_array[t_data.thread_num][i].key[0]);
			while (__sync_lock_test_and_set(&s_test_locks[list_num], 1));
			
			SortedList_insert(list[list_num], &t_data.key_array[t_data.thread_num][i]);
			__sync_lock_release(&s_test_locks[list_num]);
		}

		int i = SortedList_length(list);

		for (int i = 0; i < t_data.iterations; i++){
			list_num = hash_func(t_data.key_array[t_data.thread_num][i].key[0]);
			while (__sync_lock_test_and_set(&s_test_locks[list_num], 1));
			
			SortedList_delete(SortedList_lookup(list[list_num], t_data.key_array[t_data.thread_num][i].key));
			__sync_lock_release(&s_test_locks[list_num]);
		}
	}
	else{
		for (int i = 0; i < t_data.iterations; i++){
			list_num = hash_func(t_data.key_array[t_data.thread_num][i].key[0]);
			SortedList_insert(list[list_num], &t_data.key_array[t_data.thread_num][i]);
		}

		int i = SortedList_length(list);

		for (int i = 0; i < t_data.iterations; i++){
			list_num = hash_func(t_data.key_array[t_data.thread_num][i].key[0]);	
			SortedList_delete(SortedList_lookup(list[list_num], t_data.key_array[t_data.thread_num][i].key));
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
	{ "lists", required_argument, 0, 'e' },
    {0,0,0,0}
};


int main(int argc, char** argv){
    long long threads, iterations;
    threads = iterations = 1;
    int i = 0;
    long long operations, avglen;
    double per_op;
    long long time_init, time_finish;
    char option;
	numlists = 1;
	opt_yield = 0;
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

			case 'e':
				if ((numlists = atoi(optarg)) == 0){
					fprintf(stderr, "Error: must have positive number of lists\n");
					exit(1);
				}
				break;
                
            default:
                fprintf(stderr, "Error: Invalid argument\n");
                break;
        }
    }
	m_test_mutexs = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t)*numlists);
	s_test_locks = (int*)malloc(sizeof(int)*numlists);
	for (int i = 0; i < numlists; i++){
		pthread_mutex_init(&m_test_mutexs[i], NULL);
		s_test_locks[i] = 0;
	}
	SortedListElement_t **arr = (SortedListElement_t **)malloc( threads*sizeof(SortedListElement_t *));
    for(int i=0;i<threads;i++){
      arr[i]=(SortedListElement_t*)malloc(iterations*sizeof(SortedListElement_t));
    }
    
    // initialize empty list
    list = (SortedList_t **)malloc(sizeof(SortedList_t*)*numlists);
	for (int i = 0; i < numlists; i++){
		list[i] = (SortedList_t *)malloc(sizeof(SortedList_t));
		list[i]->next = NULL;
		list[i]->prev = NULL;
		list[i]->key = NULL;
	}
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
      fprintf(stdout,"Error: %d length\n", SortedList_length(list));
    
    
    clock_gettime(CLOCK_MONOTONIC, &timer);
    free(tids);
    time_finish = timer.tv_sec * 1000000000 + timer.tv_nsec - time_init;
    if (iterations == 1)
        avglen = 1;
    else
        avglen = iterations / 2;
    operations = threads* iterations * 2 * avglen;
    fprintf(stdout, "%d threads x %d iterations x (ins + lookup/del) x (%d/2 avg len) = %ld operations\n", threads, iterations, iterations, operations);
    if (counter != 0){ // don't need this???
        fprintf(stderr, "Error: final count = %lld\n", counter);
    }
    fprintf(stdout, "elapsed time: %lld\n", time_finish);
    per_op = time_finish / operations;
    fprintf(stdout, "per operation: %f\n", per_op);
	return 0;
}
