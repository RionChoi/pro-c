#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void) {
    // Simple number guessing game using dynamic memory for attempts tracking
    int max = 100;
    int attempts = 5;
    int *history = malloc(attempts * sizeof(int));
    if (!history) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }
    srand((unsigned)time(NULL));
    int target = rand() % max + 1;
    printf("Guess a number between 1 and %d (you have %d attempts)\n", max, attempts);
    for (int i = 0; i < attempts; ++i) {
        int guess;
        if (scanf("%d", &guess) != 1) {
            fprintf(stderr, "Input error\n");
            free(history);
            return 1;
        }
        history[i] = guess;
        if (guess == target) {
            printf("Correct! You guessed in %d attempts.\n", i+1);
            free(history);
            return 0;
        } else if (guess < target) {
            printf("Higher!\n");
        } else {
            printf("Lower!\n");
        }
    }
    printf("Out of attempts. The number was %d.\n", target);
    free(history);
    return 0;
}
