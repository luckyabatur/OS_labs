#define ARRAY_SIZE 10


static void swap(int* a, int* b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}


static int partition(int arr[], int low, int high)
{
    int p = arr[low];
    int i = low;
    int j = high;

    while (i < j)
    {
        while (arr[i] <= p && i <= high - 1)
            i++;

        while (arr[j] > p && j >= low + 1)
            j--;

        if (i < j)
            swap(&arr[i], &arr[j]);

    }
    swap(&arr[low], &arr[j]);
    return j;
}


static void quickSort(int arr[], int low, int high) {
    if (low < high)
    {
        int pi = partition(arr, low, high);
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}


#ifdef __cplusplus
extern "C"
#endif
int* Sort(int *array)
{
    quickSort(array, 0, ARRAY_SIZE-1);
    return array;
}

