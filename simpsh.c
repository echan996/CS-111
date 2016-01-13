#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <getopt.h>
#include <unistd.h>

struct file_info{
    int descriptor, readable, writable;
};
struct file_info* open_files;
int maxfiles=10, curfiles=0;
int verbose = 0;
int errors = 0;

int file_in_array(int a){
	for (int i = 0; i < curfiles; i++)
		if (a == open_files[i].descriptor) return i;
	return -1;
}
int main(int argc, char **argv){
    int numArgs = 0, start;
	open_files = (struct file_info*)malloc(maxfiles*sizeof(struct file_info));
	static struct option long_options[] = {
			{ "rdonly", required_argument, 0, 'a'},
			{ "wronly", required_argument, 0, 'b'},
			{ "command", required_argument, 0 ,'c'},
			{ "verbose", no_argument, 0, 'd'},
			{0,0,0,0}
	};
    
    char option = 0;
    int i = 0, a;
    while ((option = (char)getopt_long(argc, argv, "", long_options, &i)) != -1)
    {
		if (verbose){
			fprintf(stdout, "%s", long_options[i].name);
			if (optarg){
				for (int i = optind - 1; i < argc && argv[i][0] != '-' && argv[i][1] != '-'; i++)
					fprintf(stdout, " %s", argv[i]);
			}
			fprintf(stdout, "\n");
		}
        switch (option){
            case 'a':
                if((argv[optind-1][0] == '-' && argv[optind-1][1] == '-') || (optind < argc && argv[optind][0] != '-' && argv[optind][1] != '-'))
                {
                    optind--;
                    fprintf(stderr, "Error: Wrong number of operands\n");
                    break;
                }
				if (curfiles >= maxfiles){
					open_files = (struct file_info*)realloc(open_files, (maxfiles *= 2)*sizeof(struct file_info));
					if (open_files == NULL){
						fprintf(stderr, "Error: Unable to reallocate memory; file was not opened.\n");
						break;
					}
				}
               
                a = open(optarg, O_RDONLY);
                
                if (a == -1){
					errors++;
					fprintf(stderr, "Error: Unable to open file\n");
					break;
                 }
					
				open_files[curfiles].descriptor=a;
				open_files[curfiles].readable = 1;
				open_files[curfiles].writable = 0;
				curfiles++;
                //some storage of the open value into some global int array.
                break;
            case 'b':


				if ((argv[optind - 1][0] == '-' && argv[optind - 1][1] == '-') || (optind < argc && argv[optind][0] != '-' && argv[optind][1] != '-'))
				{
					optind--;
					fprintf(stderr, "Error: Wrong number of operands\n");
					break;
				}
				if (curfiles >= maxfiles){
					open_files = (struct file_info*)realloc(open_files, (maxfiles *= 2)*sizeof(struct file_info));
					if (open_files == NULL){
						fprintf(stderr, "Error: Unable to reallocate memory; file was not opened.\n");
						break;
					}
				}

				a = open(optarg, O_WRONLY);

				if (a == -1){
					errors++;
					fprintf(stderr, "Error: Unable to create file\n");
					break;
				}

				open_files[curfiles].descriptor = a;
				open_files[curfiles].readable = 0;
				open_files[curfiles].writable =	1;
				curfiles++;

                break;
            case 'c':
				for (int i = optind - 1; i < (optind + 2) && i < argc; i++){
					if (!isdigit(argv[i])){
						fprintf(stderr, "Error: Invalid file descriptor argument\n");
						break;
					}
						
					if (file_in_array(atoi(argv[i]))==-1){
						fprintf(stderr, "Error: File not open\n");
						break;
					}
				}
                pid_t childPID;
				if ((childPID = fork()) < 0){
					fprintf(stderr, "Error: Unable to fork child process\n");
                    break;
				}
				
                else if (childPID == 0){
                    int stdinFilePos = file_in_array(atoi(argv[optind-1]));
                    int stdoutFilePos = file_in_array(atoi(argv[optind]));
                    int stderrFilePos = file_in_array(atoi(argv[optind+1]));
                    if (!open_files[stdinFilePos].readable || !open_files[stdoutFilePos].writable || !open_files[stderrFilePos].writable){
                        fprintf(stderr, "Error: File permission denied\n");
                        break;
                    }
					int a;
					for (a = optind - 2; a < argc && argv[a][0] == '-' && argv[a][1] != '\0'; a++)
						continue;

                    dup2(atoi(argv[optind-1]), STDIN_FILENO);
                    dup2(atoi(argv[optind]), STDOUT_FILENO);
                    dup2(atoi(argv[optind+1]), STDERR_FILENO);
                    close(atoi(argv[optind-1]));
                    close(atoi(argv[optind]));
                    close(atoi(argv[optind+1]));
                    
                    if ((optind + 2) < argc && argv[optind+2][0] != '-' && argv[optind+2][1] != '-')
                        execvp(argv[optind+2], argv);
                    else{
                        fprintf(stderr, "Error: Invalid arguments\n");
                        break;
                    }
                    
				}

				//execvp(, dup2);
				
				//have a getopt thing parse in here looking for params that begin with - and only -.

				

                break;
            case 'd':
                verbose = 1;
                break;
            default:
                break;
        }
       
    }
}


