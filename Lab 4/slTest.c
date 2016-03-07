#include <getopt.h>
#include "SortedList.c"

static struct option long_options[] = {
		{ "--threads", required_argument, 0, 'a' },
		{ "--iter", required_argument, 0, 'b' },
		{ "--iterations", required_argument, 0, 'b' },
		{ "--yield", optional_argument, 0, 'c' },
		{0,0,0,0}
};

int main(int argc, char** argv){
	int threads, iterations;
	threads = iterations = 1;
	int i = 0;
	char option;
	while ((option = (char)getopt_long(argc, argv, "", long_options, &i)) != -1){
		switch (option){
		case 'a':
			if (threads=atoi(optarg) == 0){
				fprintf(stderr, "Error: Argument must be positive integer\n");
			}
			break;
		case 'b':
			if (iterations = atoi(optarg) == 0){
				fprintf(stderr, "Error: Argument must be positive integer\n");
			}
			break;
		}
		case 'c':
			// error checking?
			// appropriate values depending on if i, d and/or s are specified
			break;
	}

}