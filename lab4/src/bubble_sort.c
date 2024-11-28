#define ARRAY_SIZE 10

#ifdef __cplusplus
extern "C"
#endif
int* Sort(int *array)
{
    int c, d, t;

    for (c = 0; c < (ARRAY_SIZE - 1); c++)
    {
        for (d = 0; d < ARRAY_SIZE - c - 1; d++)
        {
            if (array[d] > array[d+1])
            {
                t = array[d];
                array[d] = array[d+1];
                array[d+1] = t;
            }
        }
    }

    return array;
}

