#include <stdio.h>
#include <stdlib.h>
void option_parser(const char ** operation, const int * values){
	int i;
	char* option = operation[values[0]];
	if(option== "--rdonly"){
	}
	else if (option == "--wronly"){}
	else if (option=="--command"){}
	else if (option == "--verbose"){}
}


int main(int argc, char **argv){
	for (int i = 1; i < argc; i++){
		//parse for options, eg find values in argv that begin with --
	}
}

