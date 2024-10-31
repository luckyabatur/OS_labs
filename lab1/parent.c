#include "stdio.h"
#include "stdlib.h" // pid_t declaration is here
#include "unistd.h" // fork() and getpid() declarations are here
#include <fcntl.h>
#include "sys/wait.h"



int create_process()
{
    pid_t pid = fork();
    if (-1 == pid)
    {
        perror("fork");
        exit(-1);
    }
    return pid;
}


int main()
{

    char file_name[101];
    scanf("%100s", file_name);
    int d = open(file_name, O_RDONLY);

    if (d == -1)
    {
        perror("open failed");
        return 1;
    }

    int pipe1[2];

    if (pipe(pipe1) == -1)
    {
        close(d);
        perror("pipe failed");
        return 2;
    }

    pid_t pid = create_process();

    if (pid == 0)
    {
        close(pipe1[0]);

        if (dup2(d, STDIN_FILENO) == -1)
        {
            perror("dup2 failed");
            return 3;
        }

        close(d);

        if (dup2(pipe1[1], STDOUT_FILENO) == -1)
        {
            perror("dup2 failed");
            return 3;
        }

        execl("daughter", "daughter", NULL);
        perror("execl() failed");
        return 4;
    }
    else
    {
        close(d);
        close(pipe1[1]);
        char number [1000];
        char symbol;
        int i = 0;
        size_t status;

        while ((status = read(pipe1[0], &symbol, sizeof(symbol))) > 0)
        {
            if (status == -1)
            {
                perror("read failed");
                break;
            }

            if (symbol == '\n')
            {
                number[i] = 0;
                puts(number);
                i = 0;
            }
            else
            {
                number[i++] = symbol;
            }
        }
        close(pipe1[0]);
        wait(NULL);
    }
    return 0;
}