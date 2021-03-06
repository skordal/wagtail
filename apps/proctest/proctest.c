// Wagtail Test Application: fork, waitpid and exit syscalls
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail/issues>

#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char * argv[])
{
	pid_t retval = fork();

	if(retval == 0)
	{
		for(int i = 0; i < 10; ++i)
			asm volatile("nop\n\t");
		exit(2);
	} else {
		int status;
		waitpid(retval, &status, 0);
		exit(1);
	}

	return 0;
}

