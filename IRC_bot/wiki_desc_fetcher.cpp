#include <unistd.h>
#include <sys/wait.h>
int main()
{
	pid_t ch_pid;
	char *args[] = {"/usr/bin/curl","https://en.wikipedia.org/w/api.php?format=json&action=query&prop=extracts&exintro&explaintext&redirects=1&titles=Carrot", NULL};
	ch_pid = fork();
	if (ch_pid == -1)
	{
	}
	else if (ch_pid > 0)
	{

	}
	else
	{
		execve(args[0], args, NULL);
        return 1;
	}

	return 0;
}
