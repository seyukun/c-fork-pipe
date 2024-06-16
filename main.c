#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

static int  await(pid_t pid, int infd, int outfd)
{
    int stat;

    if (waitpid(pid, &stat, 0) == pid)
    {
        if (WIFEXITED(stat))
            stat = WEXITSTATUS(stat);
        else if (WIFSIGNALED(stat))
            stat = WTERMSIG(stat);
        if (infd != STDIN_FILENO)
            close(infd);
        if (outfd != STDOUT_FILENO)
            close(outfd);
        return (stat);
    }
    return (1);
}

int execute(char *const *argv, char *const *envp, int infd, int outfd)
{
    pid_t   pid;

    if (argv == NULL || argv[0] == NULL)
        return (0);
    pid = fork();
    if (pid == -1)
        exit(1);
    else if (pid == 0 && dup2(infd, STDIN_FILENO) != -1
        && dup2(outfd, STDOUT_FILENO) != -1 && execve(argv[0], argv, envp))
        exit(1);
    else
        return (await(pid, infd, outfd));
    return (1);
}

int main(void)
{
    int         pipefd[2];
    char *const do1[] = {"/bin/ls", "-la", NULL};
    char *const do2[] = {"/bin/cat", NULL};
    char *const empty[] = {NULL};

    pipe(pipefd);
    execute(do1, empty, STDIN_FILENO, pipefd[1]);
    printf("done\n");
    execute(do2, empty, pipefd[0], STDOUT_FILENO);
    return (0);
}
