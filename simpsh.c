#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
int* open_files[100];
int verbose = 0;
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
		if (length != 2)
			return;
		int a = open(argv[start + 1], O_RDONLY);
		if (a == -1){
			fprintf(stderr, "Error: Unable to create file");
			exit(1);
		}
		//some storage of the open value into some global int array.
	}
	else if (option == "--wronly"){
		if (length != 2)
			return;
		int a = open(argv[start + 1], O_WRONLY);
		if (a == -1){
			fprintf(stderr, "Error: Unable to create file");
			exit(1);
		}
	}
	else if (option=="--command"){
	
	}
	else if (option == "--verbose"){
		verbose = 1;
	}
}

int main(int argc, char **argv){
    int numArgs = 0, start;
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


