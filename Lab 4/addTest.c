#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
void add(long long *pointer, long long value) {
	long long sum = *pointer + value;
	*pointer = sum;
}

static struct option long_options[] = {
		{ "--threads", required_argument, 0, 'a' },
		{ "--iter", required_argument, 0, 'b' },
		{ "--iterations", required_argument, 0, 'b' },
		{0,0,0,0}
};


int main(int argc, char** argv){
	
}