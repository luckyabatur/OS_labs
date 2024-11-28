#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/mman.h>
#include <stdbool.h>
#include <semaphore.h>

#define PAGE_SIZE 4096
#define STRING_LENGTH 100

int main()
{
    sem_unlink("/mysem");
    sem_t *sem = sem_open("/mysem", O_CREAT | O_EXCL, 0666, 0);
    if (sem == SEM_FAILED)
    {
        perror("sem_open failed");
        return 1;
    }

    int temp_fd = open("temp", O_RDWR | O_CREAT | O_TRUNC, 0666);
    if (temp_fd == -1)
    {
        perror("open failed");
        sem_close(sem);
        sem_unlink("/mysem");
        return 2;
    }

    if (ftruncate(temp_fd, PAGE_SIZE) == -1)
    {
        perror("ftruncate failed");
        close(temp_fd);
        sem_close(sem);
        sem_unlink("/mysem");
        unlink("temp");
        return 3;
    }

    char* shared_mem = mmap(0, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, temp_fd, 0);
    if (shared_mem == MAP_FAILED)
    {
        perror("mmap failed");
        close(temp_fd);
        sem_close(sem);
        sem_unlink("/mysem");
        unlink("temp");
        return 4;
    }

    char name[STRING_LENGTH];
    scanf("%99s", name);

    int fd = open(name, O_RDONLY);
    if (fd == -1)
    {
        perror("open failed");
        munmap(shared_mem, PAGE_SIZE);
        close(temp_fd);
        sem_close(sem);
        sem_unlink("/mysem");
        unlink("temp");
        return 5;
    }

    pid_t pid = fork();
    if (pid == -1)
    {
        perror("fork");
        close(fd);
        munmap(shared_mem, PAGE_SIZE);
        close(temp_fd);
        sem_close(sem);
        sem_unlink("/mysem");
        unlink("temp");
        return 6;
    }

    if (pid == 0)
    {
        if (dup2(fd, STDIN_FILENO) == -1)
        {
            perror("dup2 failed");
            close(fd);
            munmap(shared_mem, PAGE_SIZE);
            close(temp_fd);
            sem_close(sem);
            sem_unlink("/mysem");
            unlink("temp");
            return 7;
        }
        close(fd);

        execl("child", "child", NULL);

        perror("execl() failed");
        munmap(shared_mem, PAGE_SIZE);
        close(temp_fd);
        sem_close(sem);
        sem_unlink("/mysem");
        unlink("temp");
        return 8;
    }
    else
    {
        close(fd);

        int i = 0;

        while (true)
        {
            sem_wait(sem);
            char symbol = shared_mem[i++];
            if (symbol == 0)
                break;
            putchar(symbol);
        }

        int status;

        if (waitpid(pid, &status, 0) == -1)
        {
            perror("Ошибка waitpid");
            return 9;
        }

        if (WIFEXITED(status)) {
            int child_code = WEXITSTATUS(status);

            if (child_code == 1)
                perror("open child failed");
            else if (child_code == 2)
                perror("mmap child failed");
            else if (child_code == 3)
                perror("sem_open child failed");
        }

        munmap(shared_mem, PAGE_SIZE);
        close(temp_fd);
        sem_close(sem);
        sem_unlink("/mysem");
        unlink("temp");
    }

    return 0;
}