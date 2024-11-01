#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include <string.h>
#include <stdbool.h>


bool prime(int n)
{
    for(int i = 2; i * i <= n; i++)
        if(n % i == 0)
            return false;
    return true;
}

int main()
{
    char number[1000];
    char symbol;
    int i = 0;
    while ((read(STDIN_FILENO, &symbol, sizeof(symbol))) > 0)
    {
        if (symbol == '\n')
        {
            number[i] = 0;
            if (prime(atoi(number)))
            {
                return 0;
            }
            write(STDOUT_FILENO, number, strlen(number));
            write(STDOUT_FILENO, "\n", 1);
            i = 0;
        }
        else
        {
            number[i++] = symbol;
        }
    }
    return 0;
}
