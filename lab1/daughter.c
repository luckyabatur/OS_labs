#include <stdio.h>
#include <unistd.h>
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
    int number;
    while (scanf("%d", &number) != EOF)
    {
        if (prime(number))
            return 0;
        else
            printf("%d\n", number);
    }
    return 0;
}
