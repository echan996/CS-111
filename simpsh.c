#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <getopt.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

struct file_info{
    int descriptor, readable, writable, open;
};
struct file_info* open_files;
int maxfiles=10, curfiles=0;
int verbose = 0;
int errors = 0; //deal with after
int maxExit = 0;
int append=0, cloexec=0, creat=0, directory=0, dsyc=0, excl=0, nofollow=0, nonblock=0, rsync=0, sync=0,trunc=0;
void sig_handler(int signum){
	fprintf(stderr, "%d caught\n", signum);
	exit(signum);
};

int main(int argc, char **argv){
	int numArgs = 0, start;
	open_files = (struct file_info*)malloc(maxfiles*sizeof(struct file_info));
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
			{ 0, 0, 0, 0 }
	};

	char option = 0;
	int i = 0, a;
	while ((option = (char)getopt_long(argc, argv, "", long_options, &i)) != -1)
	{
		if (verbose){
			fprintf(stdout, "%s", long_options[i].name);
			if (optarg){
				for (int i = optind - 1; i < argc && (argv[i][0] != '-' || argv[i][1] != '-'); i++)
					fprintf(stdout, " %s", argv[i]);
			}
			fprintf(stdout, "\n");
		}//change this in a bit
		switch (option){
		case 'a':
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

			a = open(optarg, O_RDONLY | (O_APPEND & append) | (O_CLOEXEC & cloexec) | (O_CREAT & creat) | (O_DIRECTORY & directory) | (O_DSYC & dsyc) | (O_EXCL & excl) | (O_NOFOLLOW & nofollow)
			| (O_NONBLOCK & nonblock) | (O_RSYNC & rsync) | (O_SYNC & sync) | (O_TRUNC & trunc));

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
			//some storage of the open value into some global int array.
			break;
		case 'b':


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


			a = open(optarg, O_WRONLY | (O_APPEND & append) | (O_CLOEXEC & cloexec) | (O_CREAT & creat) | (O_DIRECTORY & directory) | (O_DSYC & dsyc) | (O_EXCL & excl) | (O_NOFOLLOW & nofollow)
				| (O_NONBLOCK & nonblock) | (O_RSYNC & rsync) | (O_SYNC & sync) | (O_TRUNC & trunc));

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

			break;
		case 'p':
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


			a = open(optarg, O_RDWR | (O_APPEND & append) | (O_CLOEXEC & cloexec) | (O_CREAT & creat) | (O_DIRECTORY & directory) | (O_DSYC & dsyc) | (O_EXCL & excl) | (O_NOFOLLOW & nofollow)
				| (O_NONBLOCK & nonblock) | (O_RSYNC & rsync) | (O_SYNC & sync) | (O_TRUNC & trunc));

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

			break;
		case 'c':{
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
			
            
            int openCheck = 0;
            for (int i = oldoptind - 1; i < (oldoptind + 2) && i < argc; i++){
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
                if (d == -1 || b == -1 || c == -1){
					fprintf(stderr, "Error: unable to open file");
                    errors++;
                    break;
                }
				argv[optind] = '\0';
				execvp(argv[oldoptind+2], &argv[oldoptind+2]);

			}
			else{
				int status;
				int returnedPid=waitpid(childPID, &status, WNOHANG);
                if (status > maxExit)
                    maxExit = status;

				break;
				/*int parentPID = fork();
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
				}*/
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
			creat = -1;
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
			sync = -1;
			break;
		case 'o':
			trunc = -1;
			break;
		case 'q':
			if (atoi(optarg) >= curfiles){
				fprintf(stderr, "Error: File never opened\n");
				break;
			}
			open_files[atoi(optarg)].open = 0;
			break;
		case 'r':
			int *a = NULL;
			int b = *a;
			break;
		case 's'://catch
			signal(atoi(optarg), sig_handler);
			break;
		case 't'://ignore
			signal(atoi(optarg), SIG_IGN);
			break;
		case 'u'://default
			signal(atoi(optarg), SIG_DFL);
			break;

		default:
			break;
		}


		
	}
	free(open_files);
    exit(maxExit);
 }


	