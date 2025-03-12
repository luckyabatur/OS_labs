#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <semaphore.h>

#define PAGE_SIZE 4096
#define STRING_MAX_SIZE 10000
#define NUMBER_MAX_SIZE 15

bool prime(int n)
{
    if (n <= 0)
        return true;
    for(int i = 2; i * i <= n; i++)
        if(n % i == 0)
            return false;
    return true;
}

int main()
{
    int fd = open("temp", O_RDWR);
    if (fd == -1)
    {
        perror("open child failed: ");
        return 1;
    }

    char* shared_mem = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shared_mem == MAP_FAILED)
    {
        perror("mmap child failed: ");
        close(fd);
        return 2;
    }

    sem_t *sem = sem_open("/mysem", 0);
    if (sem == SEM_FAILED)
    {
        perror("sem_open child failed: ");
        munmap(shared_mem, PAGE_SIZE);
        close(fd);
        return 3;
    }

    char numbers_string[STRING_MAX_SIZE];
    char number[NUMBER_MAX_SIZE];
    char symbol;
    int i = 0, j = 0, k = 0, length = 0;

    length = read(STDIN_FILENO, numbers_string, STRING_MAX_SIZE);
    if (length == -1)
    {
        perror("sem_open read failed: ");
        sem_post(sem);
        sem_close(sem);
        munmap(shared_mem, PAGE_SIZE);
        close(fd);
        return 4;
    }

    for (; j < length; j++)
    {
        symbol = numbers_string[j];
        if (symbol == '\n')
        {
            number[i] = 0;
            if (prime(atoi(number)))
            {
                shared_mem[k] = 0;
                sem_post(sem);
                sem_close(sem);
                munmap(shared_mem, PAGE_SIZE);
                close(fd);
                return 0;
            }
            for (; k <= j; ++k)
            {
                shared_mem[k] = numbers_string[k];
                sem_post(sem);
            }
            k=j+1;
            i = 0;
        }
        else
        {
            number[i++] = symbol;
        }
    }
    shared_mem[++j] = 0;

    sem_post(sem);
    sem_close(sem);
    munmap(shared_mem, PAGE_SIZE);
    close(fd);
    return 0;
}
