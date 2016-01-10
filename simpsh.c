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
		if (verbose){
			fprintf(stdout, "%s", long_options[i]);
			if (optarg){
				for (int i = optind - 1; argv[i][0] != '-' && argv[i][1] != '-'; i++)
					fprintf(stdout, " %s", argv[i]);
			}
			fprintf(stdout, "\n");
		}
        switch (option){
            case 'a':
                if((argv[optind-1][0] == '-' && argv[optind-1][1] == '-') || (optind < argc && argv[optind][0] != '-' && argv[optind][1] != '-'))
                {
                    optind--;
                    fprintf(stderr, "Wrong number of operands\n");
                    break;
                }
				if (curfiles >= maxfiles){
					open_files = (int*)realloc(open_files, (maxfiles *= 2)*sizeof(int));
					if (open_files == NULL){
						fprintf(stderr, "Unable to reallocate memory; file was not opened.\n");
						break;
					}
				}
               
                int a = open(optarg, O_RDONLY);
                
                if (a == -1){
					errors++;
					fprintf(stderr, "Error: Unable to create file\n");
					break;
                 }
					
				openfiles[curfiles++];
                
                //some storage of the open value into some global int array.
                fprintf(stdout, "rdonly option\n");
                break;
            case 'b':


				if ((argv[optind - 1][0] == '-' && argv[optind - 1][1] == '-') || (optind < argc && argv[optind][0] != '-' && argv[optind][1] != '-'))
				{
					optind--;
					fprintf(stderr, "Wrong number of operands\n");
					break;
				}
				if (curfiles >= maxfiles){
					open_files = (int*)realloc(open_files, (maxfiles *= 2)*sizeof(int));
					if (open_files == NULL){
						fprintf(stderr, "Unable to reallocate memory; file was not opened.\n");
						break;
					}
				}

				int a = open(optarg, O_WRONLY);

				if (a == -1){
					errors++;
					fprintf(stderr, "Error: Unable to create file\n");
					break;
				}

				openfiles[curfiles++];
                fprintf(stdout, "wronly option\n");
                break;
            case 'c':
                fprintf(stdout, "command option\n");
                break;
            case 'd':
                fprintf(stdout, "verbose option\n");
                verbose = 1;
                break;
            default:
                fprintf(stdout, "default case\n");
                break;
        }
        fprintf(stdout, "%c\n", option);
       
    }
}


