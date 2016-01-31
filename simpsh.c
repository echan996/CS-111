#define _GNU_SOURCE
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <getopt.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/resource.h>

struct file_info{
    int descriptor, readable, writable, open;
};
struct thread_info{
	int start_ind, end_ind;
	pid_t threadnum;
};
struct file_info* open_files;
struct thread_info* running_threads;
int maxfiles=10, curfiles=0, maxthreads=10, curthreads=0;
int verbose = 0, profile=0;
int errors = 0; //deal with after
int maxExit = 0;
int append=0, cloexec=0, create=0, directory=0, dsyc=0, excl=0, nofollow=0, nonblock=0, rsync=0, syc=0,trun=0;
void sig_handler(int signum){
	fprintf(stderr, "%d caught\n", signum);
	_exit(signum);
};
static struct option long_options[] = {
    { "rdonly", required_argument, 0, 'a' },
    { "wronly", required_argument, 0, 'b' },
    { "command", required_argument, 0, 'c' },
    { "verbose", no_argument, 0, 'd' },
    { "append", no_argument, 0, 'e' },
    { "cloexec", no_argument, 0, 'f' },
    { "creat", no_argument, 0, 'g' },
    { "directory", no_argument, 0, 'h' },
    { "dsyc", no_argument,0,'i' },
    { "excl", no_argument,0, 'j' },
    { "nofollow", no_argument, 0, 'k' },
    { "nonblock", no_argument, 0, 'l' },
    { "rsync", no_argument, 0, 'm' },
    { "sync", no_argument, 0, 'n' },
    { "trunc", no_argument, 0, 'o' },
    { "rdwr", required_argument, 0, 'p' },
    { "close", required_argument, 0, 'q' },
    { "abort", no_argument, 0, 'r' },
    { "catch", required_argument, 0, 's' },
    { "ignore", required_argument, 0, 't' },
    { "default", required_argument, 0, 'u' },
	{ "pause", no_argument, 0, 'v' },
	{ "wait", no_argument, 0, 'w' },
    { "pipe", no_argument, 0, 'x' },
	{ "profile",no_argument,0,'y' },
    { 0, 0, 0, 0 }
};

void verbosePrint(char option, int optind, char **argv, int longOptInd, int argc){
    if (verbose){ // verbose is turned on --> print out the commands
        int e, j;
        for (e = optind - 1; e > -1 && argv[e][0] != '-' && argv[e][1] != '-'; e--){ // find the index of the option within argv
            continue;
        }
        int index = e;
        if (option == 'a' || option == 'b' || option == 'p'){ // need to add pipe
            // go back until you find a non-file flag
            for (j = index - 1; j > 0 && (!(strcmp(argv[j], "--append")) || !(strcmp(argv[j], "--cloexec")) || !(strcmp(argv[j], "--creat")) || !(strcmp(argv[j], "--directory")) || !(strcmp(argv[j], "--dsync")) || !(strcmp(argv[j], "--excl")) || !(strcmp(argv[j], "--nofollow")) || !(strcmp(argv[j], "--nonblock")) || !(strcmp(argv[j], "--rsync")) || !(strcmp(argv[j], "--sync")) || !(strcmp(argv[j], "--trunc"))); j--){ // j indexes through argv
                continue;
            } // j stops at the element in argv that isn't a file flag
            
            j++;
            
            // print out all of the file flags starting from j until index
            for (; j < index; j++){
                fprintf(stdout, "%s ", argv[j]);
            }
        }
        // then print out current option name and operands
        fprintf(stdout, "--%s", long_options[longOptInd].name); // print out the option name
        if (optarg){ // null if no arguments
            int i;
            for (i = optind - 1; i < argc && (argv[i][0] != '-' || argv[i][1] != '-'); i++)
                fprintf(stdout, " %s", argv[i]);
        }
        fprintf(stdout, "\n");
    }
}

int main(int argc, char **argv){
	int numArgs = 0, start;
	open_files = (struct file_info*)malloc(maxfiles*sizeof(struct file_info));
	running_threads = (struct thread_info*)malloc(maxthreads*sizeof(struct thread_info));
	char option = 0;
	int i = 0, a;
	while ((option = (char)getopt_long(argc, argv, "", long_options, &i)) != -1)
	{
		switch (option){
		case 'a':
            verbosePrint(option, optind, argv, i, argc);
			if ((argv[optind - 1][0] == '-' && argv[optind - 1][1] == '-') || (optind < argc && argv[optind][0] != '-' && argv[optind][1] != '-'))
			{
				optind--;
				fprintf(stderr, "Error: Wrong number of operands\n");
                errors++;
				break;
			}
			if (curfiles >= maxfiles){
				open_files = (struct file_info*)realloc(open_files, (maxfiles *= 2)*sizeof(struct file_info));
				if (open_files == NULL){
					fprintf(stderr, "Error: Unable to reallocate memory; file was not opened.\n");
                    errors++;
					break;
				}
			}

			a = open(optarg, O_RDONLY | (O_APPEND & append) | (O_CLOEXEC & cloexec) | (O_CREAT & create) | (O_DIRECTORY & directory) | (O_DSYNC & dsyc) | (O_EXCL & excl) | (O_NOFOLLOW & nofollow)
			| (O_NONBLOCK & nonblock) | (O_RSYNC & rsync) | (O_SYNC & syc) | (O_TRUNC & trun));

			if (a == -1){
				errors++;
				fprintf(stderr, "Error: Unable to open file\n");
                errors++;
				break;
			}

			open_files[curfiles].descriptor = a;
			open_files[curfiles].readable = 1;
			open_files[curfiles].writable = 0;
			open_files[curfiles].open = 1;
			curfiles++;
			append = cloexec = create = directory = dsyc = excl = nofollow = nonblock = rsync = syc = trun = 0;
			//some storage of the open value into some global int array.
			break;
		case 'b':
            verbosePrint(option, optind, argv, i, argc);
			if ((argv[optind - 1][0] == '-' && argv[optind - 1][1] == '-') || (optind < argc && argv[optind][0] != '-' && argv[optind][1] != '-'))
			{
				optind--;
				fprintf(stderr, "Error: Wrong number of operands\n");
                errors++;
				break;
			}
			if (curfiles >= maxfiles){
				open_files = (struct file_info*)realloc(open_files, (maxfiles *= 2)*sizeof(struct file_info));
				if (open_files == NULL){
					fprintf(stderr, "Error: Unable to reallocate memory; file was not opened.\n");
                    errors++;
					break;
				}
			}


			a = open(optarg, O_WRONLY | (O_APPEND & append) | (O_CLOEXEC & cloexec) | (O_CREAT & create) | (O_DIRECTORY & directory) | (O_DSYNC & dsyc) | (O_EXCL & excl) | (O_NOFOLLOW & nofollow)
				| (O_NONBLOCK & nonblock) | (O_RSYNC & rsync) | (O_SYNC & syc) | (O_TRUNC & trun));

			if (a == -1){
				errors++;
				fprintf(stderr, "Error: Unable to create file\n");
                errors++;
				break;
			}

			open_files[curfiles].descriptor = a;
			open_files[curfiles].readable = 0;
			open_files[curfiles].writable = 1;
			open_files[curfiles].open = 1;
			curfiles++;
			append = cloexec = create = directory = dsyc = excl = nofollow = nonblock = rsync = syc = trun = 0;
			break;
		case 'p':
            verbosePrint(option, optind, argv, i, argc);
			if ((argv[optind - 1][0] == '-' && argv[optind - 1][1] == '-') || (optind < argc && argv[optind][0] != '-' && argv[optind][1] != '-'))
			{
				optind--;
				fprintf(stderr, "Error: Wrong number of operands\n");
				errors++;
				break;
			}
			if (curfiles >= maxfiles){
				open_files = (struct file_info*)realloc(open_files, (maxfiles *= 2)*sizeof(struct file_info));
				if (open_files == NULL){
					fprintf(stderr, "Error: Unable to reallocate memory; file was not opened.\n");
					errors++;
					break;
				}
			}


			a = open(optarg, O_RDWR | (O_APPEND & append) | (O_CLOEXEC & cloexec) | (O_CREAT & create) | (O_DIRECTORY & directory) | (O_DSYNC & dsyc) | (O_EXCL & excl) | (O_NOFOLLOW & nofollow)
				| (O_NONBLOCK & nonblock) | (O_RSYNC & rsync) | (O_SYNC & syc) | (O_TRUNC & trun));

			if (a == -1){
				errors++;
				fprintf(stderr, "Error: Unable to create file\n");
				errors++;
				break;
			}

			open_files[curfiles].descriptor = a;
			open_files[curfiles].readable = 1;
			open_files[curfiles].writable = 1;
			open_files[curfiles].open = 1;
			curfiles++;
			append = cloexec = create = directory = dsyc = excl = nofollow = nonblock = rsync = syc = trun = 0;
			break;

			//COMMAND
		case 'c':{
            verbosePrint(option, optind, argv, i, argc);
			int index, count=0;
            int oldoptind = optind;
			for (index = optind - 1; index < argc; index++, count++){
				if (argv[index][0] == '-' && argv[index][1] == '-')
					break;
			}
            optind = index;
			if (count < 4){
				fprintf(stderr, "Error: --command requires at least 4 options.\n");
                errors++;
				break;
			}
			
            
            int openCheck = 0, i;
            for (i = oldoptind - 1; i < (oldoptind + 2) && i < argc; i++){
                if (atoi(argv[i]) >= curfiles || open_files[atoi(argv[i])].open==0){
                    openCheck = 1;
                    break;
                }
            }
            if (openCheck){
                fprintf(stderr, "Error: File not open\n");
                errors++;
                break;
            }
			
			int stdinFilePos = atoi(argv[oldoptind - 1]), stdoutFilePos = atoi(argv[oldoptind]), stderrFilePos = atoi(argv[oldoptind + 1]);
			/*if (!open_files[stdinFilePos].readable || !open_files[stdoutFilePos].writable || !open_files[stderrFilePos].writable){
				fprintf(stderr, "Error: File permission denied\n");
				break;
			}
			*/
			
			////////////////////////////////////////////////////////////////////////Executable Processing////////////////////////////////////////////////////////////////////

			int childPID = fork();
			//int fd[2];
			//pipe(fd);

			if (childPID < 0){
				fprintf(stderr, "Error: Unable to fork child process\n");
                errors++;
                break;
			}

			else if (childPID == 0){
				//close(fd[1]);
				int b=dup2(open_files[stdinFilePos].descriptor, STDIN_FILENO), c=dup2(open_files[stdoutFilePos].descriptor, STDOUT_FILENO), d=dup2(open_files[stderrFilePos].descriptor, STDERR_FILENO);
                int j;
                for (j = 0; j < curfiles; j++){
                    open_files[j].open = 0;
                    close(open_files[j].descriptor);
                }
                if (d == -1 || b == -1 || c == -1){
					fprintf(stderr, "Error: unable to open file");
                    errors++;
                    break;
                }
				argv[optind] = '\0';

				if (curfiles >= maxfiles){
					open_files = (struct file_info*)realloc(open_files, (maxfiles *= 2)*sizeof(struct file_info));
					if (open_files == NULL){
						fprintf(stderr, "Error: Unable to reallocate memory; file was not opened.\n");
						errors++;
						break;
					}
				}
			
				
				execvp(argv[oldoptind+2], &argv[oldoptind+2]);

			}
			else{
				/*int status;
				int returnedPid=waitpid(childPID, &status, 0);
                if (status > maxExit)
                    maxExit = status;*/ //removed exit status because they get reaped which makes it so i can't access them in wait
				running_threads[curthreads].start_ind = oldoptind + 2;
				running_threads[curthreads].end_ind = optind;
				running_threads[curthreads].threadnum = childPID;
				curthreads++;
				break;
			
			}
		

		}
		case 'd':
			verbose = 1;
			break;

		//dealing with file opening options.
		case 'e':
			append = -1;
			break;
		case 'f':
			cloexec = -1;
			break;
		case 'g':
			create = -1;
			break;
		case 'h':
			directory = -1;
			break;
		case 'i':
			dsyc = -1;
			break;
		case 'j':
			excl = -1;
			break;
		case 'k':
			nofollow = -1;
			break;
		case 'l':
			nonblock = -1;
			break;
		case 'm':
			rsync = -1;
			break;
		case 'n':
			syc = -1;
			break;
		case 'o':
			trun = -1;
			break;
		case 'q':
			verbosePrint(option, optind, argv, i, argc);
			if (atoi(optarg) >= curfiles || open_files[atoi(optarg)].open==0){
				fprintf(stderr, "Error: File not opened\n");
				break;
			}
			open_files[atoi(optarg)].open = 0;
			close(open_files[atoi(optarg)].descriptor);
			break;
		case 'r':
            verbosePrint(option, optind, argv, i, argc);
            raise(SIGSEGV);
			break;
		case 's'://catch
            verbosePrint(option, optind, argv, i, argc);
			signal(atoi(optarg), sig_handler);
			break;
		case 't'://ignore
            verbosePrint(option, optind, argv, i, argc);
			signal(atoi(optarg), SIG_IGN);
			break;
		case 'u'://default
            verbosePrint(option, optind, argv, i, argc);
			signal(atoi(optarg), SIG_DFL);
			break;
		case 'v':
			verbosePrint(option, optind, argv, i, argc);
			pause();
			break;
		case 'w':
			verbosePrint(option, optind, argv, i, argc);
			for (int i = 0; i < curfiles; i++){
				if (open_files[i].open)
					close(open_files[i].descriptor);
			}
			for (int i = 0; i < curthreads; i++){
				int status;
				pid_t thrd = waitpid(-1, &status, 0);
				if (status > maxExit)
					maxExit = status;
				for (int i = 0; i < curthreads;i++)
					if (thrd == running_threads[i].threadnum){
					fprintf(stdout, "%d", status);
					for (int b = running_threads[i].start_ind; b < running_threads[i].end_ind; b++)
						fprintf(stdout, " %s", argv[b]);
					fprintf(stdout, "\n");
					}
			}
			break;
        case 'x': //pipe
            verbosePrint(option, optind, argv, i, argc);
            if (argv[optind][0] != '-' && argv[optind][1] != '-')
            {
                fprintf(stderr, "Error: Pipe should not have operands\n");
                errors++;
                break;
            }
                
            int fd[2];
            pipe(fd);
            int i;
            for (i = 0; i < 2; i++){ // add two file descriptors to the open_files array
                if (curfiles >= maxfiles){
                    open_files = (struct file_info*)realloc(open_files, (maxfiles *= 2)*sizeof(struct file_info));
                    if (open_files == NULL){
                        fprintf(stderr, "Error: Unable to reallocate memory; file was not opened.\n");
                        errors++;
                        break;
                    }
                }
                open_files[curfiles].descriptor = fd[i];
                if (i){ // when i is 0 and the read end of the pipe
                    open_files[curfiles].readable = 1;
                    open_files[curfiles].writable = 0;
                }
                else {// when i is 1 and the write end of the pipe
                    open_files[curfiles].readable = 0;
                    open_files[curfiles].writable = 1;
                }
                open_files[curfiles].open = 1;
                curfiles++;
            }
            
            break;
		case 'y':
			profile = 1;
			break;
		default:
			break;
		}


		
	}
	free(open_files);
    exit(maxExit);
 }


	