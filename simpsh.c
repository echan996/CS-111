#include <stdio.h>
#include <stdlib.h>
void option_parser(const char ** argv, const int * values){
	int i;
	char* option = operation[values[0]];
	if(option== "--rdonly"){
	}
	else if (option == "--wronly"){}
	else if (option=="--command"){}
	else if (option == "--verbose"){}
}

int main(int argc, char **argv){
    int numArgs = 0, start;
    for (int i = 1; i < argc; i++){
        if (argv[i][0] == '-' && argv[i][1] == '-') // current one is the option
            start = i;
        
        numArgs++;
        
        if (argv[i+1][0] == '-' && argv[i+1][1] == '-') // next one is the option
        {
            option_parser(argv, start, numArgs);
            numArgs = 0;
        }
    }
}


