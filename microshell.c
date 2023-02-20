#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int ft_strlen(char *str)
{
    int i = 0;
    while (str[i])
        i++;
    return (i);
}

void print_error(char *str, char *argv)
{
    if (str)
        write(2, str, ft_strlen(str));
    if (argv)
        write(2, argv, ft_strlen(argv));
    write(2, "\n", 1);
}

int ft_exec(char **argv, int i, int tmp_fd, char **env)
{
    argv[i] = NULL;
    close(tmp_fd);
    execve(argv[0], argv, env);
    print_error("error : cannot execute ", argv[0]);
    return (1);
}

int main(int argc, char **argv, char **env)
{
    (void)argc;
    int fd[2];
    int i = 0;
    int pid = 0;
    int tmp_fd = dup(STDIN_FILENO);
    while (argv[i] && argv[i+1])
    {
        argv = &argv[i+1];
        i = 0;
        while (argv[i] && strcmp(argv[i], ";") && strcmp(argv[i], "|"))
            i++;
        if (strcmp(argv[0], "cd") == 0)
        {
            if (i != 2)
                print_error("error: cd: bad arguments", NULL);
            else if(chdir(argv[1]) != 0)
                print_error("error: cd: cannot change directory to ", argv[1]);
        }
        else if (argv != &argv[i] && (argv[i] == NULL || strcmp(argv[i], ";") == 0))
        {
            pid = fork();
            if (pid == 0)
            {
                dup2(tmp_fd, STDIN_FILENO);
                if (ft_exec(argv, i, tmp_fd, env))
                    return (1);
            }
            else
            {
                close(tmp_fd);
                waitpid(-1, NULL, WUNTRACED);
                tmp_fd = dup(STDIN_FILENO);
            }
        }
        else if (argv != &argv[i] && (argv[i] == NULL || strcmp(argv[i], "|") == 0))
        {
            pipe(fd);
            pid = fork();
            if (pid == 0)
            {
                dup2(tmp_fd, STDIN_FILENO);
                dup2(fd[1], STDOUT_FILENO);
                close(fd[1]);
                close(tmp_fd);
                if (ft_exec(argv, i, tmp_fd, env))
                    return (1);
            }
            else 
            {
                close(tmp_fd);
                close(fd[1]);
                waitpid(-1, NULL, WUNTRACED);
                tmp_fd = dup(fd[0]);
                close(fd[0]);
            }
        }
    }
    close(tmp_fd);
    return(0);
}