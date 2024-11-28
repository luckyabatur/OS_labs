#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>

#define DECK_SIZE 52

void* do_experiment(void* arg) {
    int deck[DECK_SIZE];
    int success_count = 0;
    int* rounds_and_res = (int*)arg;
    int round_thread_count = rounds_and_res[0];
    unsigned int seed = time(NULL) ^ pthread_self();

    for (int i = 0; i < round_thread_count; i++) {
        for (int j = 0; j < DECK_SIZE; j++)
            deck[j] = rand_r(&seed) % 14;

        if (deck[DECK_SIZE - 2] == deck[DECK_SIZE - 1])
            success_count++;
    }

    rounds_and_res[1] = success_count;
    return NULL;
}

int main(int argc, char* argv[]) {
    struct timeval start, end;
    gettimeofday(&start, NULL);

    if (argc != 3) {
        perror("Usage: <thread_count> <max_rounds>");
        return 1;
    }

    int thread_count = atoi(argv[1]) - 1;
    if (thread_count < 2)
    {
        perror("It's necessary to have 2 and more threads");
        return 2;
    }
    int max_rounds = atoi(argv[2]);


    int base = max_rounds / thread_count;
    int dop = max_rounds % thread_count;

    int* rounds_and_res = (int*)malloc(sizeof(int) * thread_count * 2);
    if (!rounds_and_res)
    {
        perror("Memory allocation failed");
        return 3;
    }

    pthread_t* th = (pthread_t*)malloc(thread_count * sizeof(pthread_t));
    if(!th)
    {
        free(rounds_and_res);
        perror("Memory allocation failed");
        return 3;
    }

    double probability = 0.0;

    for (int i = 0; i < thread_count; i++)
    {
        rounds_and_res[2 * i] = base + ((i < dop) ? 1 : 0);
        if (pthread_create(&th[i], NULL, do_experiment, &rounds_and_res[2 * i]) != 0)
        {
            perror("Failed to create thread");
            free(rounds_and_res);
            free(th);
            return 4;
        }
    }

    for (int i = 0; i < thread_count; i++)
    {
        if (pthread_join(th[i], NULL) != 0)
        {
            perror("Failed to join thread");
            free(rounds_and_res);
            free(th);
            return 5;
        }
        probability += rounds_and_res[2 * i + 1];
    }

    free(rounds_and_res);
    free(th);

    probability = probability / max_rounds * 100.0;
    printf("Probability = %.2f%%\n", probability);

    gettimeofday(&end, NULL);
    long seconds = end.tv_sec - start.tv_sec;
    long microseconds = end.tv_usec - start.tv_usec;
    int milliseconds = (seconds * 1000) + (microseconds / 1000.0);

    printf("Program executed in %d ms.\n", milliseconds);

    return 0;
}