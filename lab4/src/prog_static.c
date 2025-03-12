#include <stdio.h>
#include <stdbool.h>

#define ARRAY_SIZE 10

#ifdef __cplusplus
extern "C"
#endif
float Derivative(float A, float deltaX);

#ifdef __cplusplus
extern "C"
#endif
int* Sort(int *array);

int main()
{
    while (true)
    {
        printf("Choose the action:\n"
               "1 (float) A (float) deltaX - find the derivative in the point A\n"
               "2 (int) el1 (int) el2... (int) el10 - sort the array with elements el1...el10\n"
               "other value - exit\n");
        int choose;

        if (scanf("%d", &choose) != 1)
            return 0;

        switch (choose)
        {
            case 1:
            {
                float A, deltaX;
                scanf("%f%f", &A, &deltaX);

                printf("derivative: %f\n", Derivative(A, deltaX));
                break;
            }
            case 2:
            {
                int array[ARRAY_SIZE];
                for (int i = 0; i < ARRAY_SIZE; i++)
                    scanf("%d", &array[i]);

                Sort(array);
                printf("sorted array: ");
                for (int i = 0; i < ARRAY_SIZE; i++)
                    printf("%d ", array[i]);
                printf("\n");
                break;
            }
            default:
                return 0;
        }
    }
}