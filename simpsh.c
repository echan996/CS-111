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
int errors = 0;//deal with after

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

			case 'c':{
				/////////////////////////////////////////////////////////////////////////////////Error Checking/////////////////////////////////////////////////////////////////////////////////////////
				for (int i = optind - 1; i < (optind + 2) && i < argc; i++){
					for (int x = 0; argv[i][x] != '\0'; x++)
						if (!isdigit(argv[i][x])){
						fprintf(stderr, "Error: Invalid file descriptor argument\n");
						break;
						}

					if (file_in_array(atoi(argv[i])) == -1){
						fprintf(stderr, "Error: File not open\n");
						break;
					}
				}

				
				int stdinFilePos = file_in_array(atoi(argv[optind - 1])), stdoutFilePos = file_in_array(atoi(argv[optind])), stderrFilePos = file_in_array(atoi(argv[optind + 1]));
				if (!open_files[stdinFilePos].readable || !open_files[stdoutFilePos].writable || !open_files[stderrFilePos].writable){
					fprintf(stderr, "Error: File permission denied\n");
					break;
                int count = 0;
                for (int e = optind + 2; e < argc && (argv[e][0] != '-' && argv[e][1] != '-'); e++, count++){
                    continue;
                }
                char ** a = (char **)malloc(sizeof(char*)*count);
                
                for (int e = optind + 2, counter = 0; counter < count; e++){
                    a[counter] = argv[e];
                }
				////////////////////////////////////////////////////////////////////////Executable Processing////////////////////////////////////////////////////////////////////

				int childPID = fork();
				int fd[2];
				pipe(fd);

				if (childPID < 0){
					fprintf(stderr, "Error: Unable to fork child process\n");
					break;
				}

				else if (childPID == 0){
					close(fd[1]);
					dup2(fd[0], STDIN_FILENO);
					dup2(open_files[stdoutFilePos].descriptor, STDOUT_FILENO);
					dup2(open_files[stderrFilePos].descriptor, STDERR_FILENO);
					execvp(a[0], a);
				}
				else{
					int parentPID = fork();
					if (parentPID == 0){
						close(fd[0]);
						dup2(open_files[stdinFilePos].descriptor, STDIN_FILENO);
						dup2(fd[1], STDOUT_FILENO);
						dup2(open_files[stderrFilePos].descriptor, STDERR_FILENO);
						execvp(a[0], a);
					}
					else{
						close(fd[0]);
						close(fd[1]);
						int status;
						int returnedPid = waitpid(-1, &status, 0);
						if (returnedPid == parentPID)
						{
							waitpid(childPID, &status, 0);
						}
						if (returnedPid == childPID)
						{
							waitpid(parentPID, &status, 0);
							free(a);
						}
					}
				}
				break; 
			}

			
			case 'd':
                verbose = 1;
                break;


            default:
                break;
        }
       
    }
}


