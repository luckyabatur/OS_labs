#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <stdbool.h>

#define ARRAY_SIZE 10
#define LIB_PATH_SIZE 30

typedef float (*der_f)(float A, float deltaX);
typedef int* (*sort_f)(int *array);

der_f derivative = NULL;
sort_f sort = NULL;
void* lib = NULL;

void choose_library(char* dll_path)
{
    if (lib != NULL)
    {
        dlclose(lib);
    }

    lib = dlopen(dll_path, RTLD_LAZY);
    if (lib == NULL)
    {
        perror(dlerror());
        exit(1);
    }

    derivative = (der_f )dlsym(lib, "Derivative");
    sort = (sort_f)dlsym(lib, "Sort");

    if (!derivative || !sort)
    {
        perror(dlerror());
        dlclose(lib);
        exit(2);
    }
}


int main() {
    choose_library("./libLib1.so");

    while (true)
    {
        printf("Choose the action:\n"
               "0 path - change library\n"
               "1 (float) A (float) deltaX - find the derivative in the point A\n"
               "2 (int) el1 (int) el2... (int) el10 - sort the array with elements el1...el10\n"
               "other value - exit\n");
        int choose;

        if (scanf("%d", &choose) != 1)
            return 0;

        switch (choose)
        {
            case 0:
            {
                char path[LIB_PATH_SIZE];
                scanf("%29s", path);
                choose_library(path);
                break;
            }
            case 1:
            {
                float A, deltaX;
                scanf("%f%f", &A, &deltaX);

                printf("derivative: %f\n", derivative(A, deltaX));
                break;
            }
            case 2: {
                int array[ARRAY_SIZE];
                for (int i = 0; i < ARRAY_SIZE; i++)
                    scanf("%d", &array[i]);

                sort(array);
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