#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <getopt.h>
int open_files[100], maxfiles=100, curfiles=0;
int verbose = 0;
int errors = 0;
void option_parser(const char ** argv,  int start, int length){
	if (verbose){
		for (int i = 0; i < length - 1; i++){
			fprintf(stdout, argv[start + i] + ' ');
		}
		fprintf(stdout, argv[start + length - 1] + '\n');
	}
	int i;
	char* option = argv[start];
	if(option== "--rdonly"){
		if (curfiles>=maxfiles)

		if (length != 2){
			errors++;
			fprintf(stderr, "Error: Wrong number of arguments\n");
			return;
		}

		int a = open(argv[start + 1], O_RDONLY);
		
		if (a == -1){
			errors++;
			fprintf(stderr, "Error: Unable to create file");
			return;
		}
		//some storage of the open value into some global int array.
	}
	else if (option == "--wronly"){
		if (length != 2){
			errors++;
			fprintf(stderr, "Error: Wrong number of arguments\n");
			return;
		}

		int a = open(argv[start + 1], O_WRONLY);
	
		if (a == -1){
			errors++;
			fprintf(stderr, "Error: Unable to create file\n");
			return;
		}
	}
	else if (option=="--command"){
		
	}
	else if (option == "--verbose"){
		if (length != 1){
			errors++;
			fprintf(stderr, "Error: Wrong number of arguments\n");
			return;
		}
		verbose = 1;
	}
}

int main(int argc, char **argv){
    int numArgs = 0, start;
	static struct option long_options[] = {
			{ "rdonly", required_argument, 0, 0},
			{ "wronly", required_argument, 0, 0},
			{ "command", optional_argument, 0 ,0},
			{ "verbose", no_argument, 0, 0},
	};
    for (int i = 1; i < argc; i++){
        if (argv[i][0] == '-' && argv[i][1] == '-') // current one is the option
            start = i;
        
        numArgs++;
        
        if (!((i+1) < argc) || argv[i+1][0] == '-' && argv[i+1][1] == '-') // next one is the option
        {
            option_parser(argv, start, numArgs);
            numArgs = 0;
        }
    }
}


