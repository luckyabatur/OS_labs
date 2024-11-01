#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include <fcntl.h>
#include "sys/wait.h"


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

    pid_t pid = fork();

    if (-1 == pid)
    {
        close(d), close(pipe1[0]), close(pipe1[1]);
        perror("fork");
        exit(-1);
    }


    if (pid == 0)
    {
        close(pipe1[0]);

        if (dup2(d, STDIN_FILENO) == -1)
        {
            close(d), close(pipe1[1]);
            perror("dup2 failed");
            return 3;
        }

        close(d);

        if (dup2(pipe1[1], STDOUT_FILENO) == -1)
        {
            close(pipe1[1]);
            perror("dup2 failed");
            return 3;
        }

        execl("child", "child", NULL);
        close(pipe1[1]);
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

        if (status == -1)
        {
            perror("read failed");
            return 5;
        }
    }
    return 0;
}