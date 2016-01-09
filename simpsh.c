#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <getopt.h>

int maxfiles=10, curfiles=0;
int* open_files;
int verbose = 0;
int errors = 0;

int main(int argc, char **argv){
    int numArgs = 0, start;
	open_files = (int*)malloc(maxfiles*sizeof(int));
	static struct option long_options[] = {
			{ "rdonly", required_argument, 0, 'a'},
			{ "wronly", required_argument, 0, 'b'},
			{ "command", required_argument, 0 ,'c'},
			{ "verbose", no_argument, 0, 'd'},
			{0,0,0,0}
	};
    
    char option = 0;
    int i = 0;
    while ((option = (char)getopt_long(argc, argv, "", long_options, &i)) != -1)
    {
        switch (option){
            case 'a':
                if(argv[optind-1][0] == '-' && argv[optind-1][1] == '-')
                {
                    optind--;
                    fprintf(stderr, "Wrong number of operands");
                    break;
                }
                /*if (curfiles >= maxfiles){
                 open_files = (int*)realloc(open_files, (maxfiles *= 2)*sizeof(int));
                 if (open_files == NULL){
                 fprintf(stderr, "Unable to reallocate memory; file was not opened.\n");
                 exit(1);
                 }
                 
                 }
                 if (optarg == 0){
                 fprintf(stderr, "Wrong number of operands\n");
                 exit(1);
                 }
                 if ()
                 int a = open(optarg, O_RDONLY);
                 
                 if (a == -1){
                 errors++;
                 fprintf(stderr, "Error: Unable to create file");
                 exit(1);
                 }*/
                
                //some storage of the open value into some global int array.
                fprintf(stdout, "rdonly option");
                break;
            case 'b':
                if(argv[optind-1][0] == '-' && argv[optind-1][1] == '-')
                {
                    optind--;
                    fprintf(stderr, "Wrong number of operands");
                    break;
                }
                fprintf(stdout, "wronly option");
                break;
            case 'c':
                fprintf(stdout, "command option");
                break;
            case 'd':
                fprintf(stdout, "verbose option");
                verbose = 1;
                break;
            default:
                fprintf(stdout, "default case");
                break;
        }
        fprintf(stdout, "\n%c\n", option);
        /*if (verbose){
         for (int i = 0; i < length - 1; i++){
         fprintf(stdout, argv[start + i] + ' ');
         }
         fprintf(stdout, argv[start + length - 1] + '\n');
         }
         
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
         }*/
    }
}


