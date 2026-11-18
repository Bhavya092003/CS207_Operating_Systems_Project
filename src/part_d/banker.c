
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_PROCESSES 100
#define MAX_RESOURCES 100

typedef struct {
    int allocation[MAX_RESOURCES];
    int maximum[MAX_RESOURCES];
    int need[MAX_RESOURCES];
    bool finished;
} Process;

static bool can_execute(const Process *process, const int work[],
                        int resource_count) {
    for (int j = 0; j < resource_count; j++) {
        if (process->need[j] > work[j]) {
            return false;
        }
    }

    return true;
}

static void print_matrix(const char *title, Process processes[],
                         int process_count, int resource_count,
                         int use_need) {
    printf("\n%s\n", title);
    printf("Process");

    for (int j = 0; j < resource_count; j++) {
        printf("\tR%d", j);
    }

    printf("\n");

    for (int i = 0; i < process_count; i++) {
        printf("P%d", i);

        for (int j = 0; j < resource_count; j++) {
            int value = use_need ? processes[i].need[j]
                                 : processes[i].allocation[j];
            printf("\t%d", value);
        }

        printf("\n");
    }
}

static bool safety_algorithm(Process processes[],
                             int process_count,
                             int resource_count,
                             const int available[],
                             int safe_sequence[]) {
    int work[MAX_RESOURCES];

    for (int j = 0; j < resource_count; j++) {
        work[j] = available[j];
    }

    for (int i = 0; i < process_count; i++) {
        processes[i].finished = false;
    }

    int completed = 0;

    printf("\nSafety Algorithm Execution:\n");
    printf("----------------------------------------\n");

    while (completed < process_count) {
        bool found = false;

        for (int i = 0; i < process_count; i++) {
            if (!processes[i].finished &&
                can_execute(&processes[i], work, resource_count)) {

                printf("P%d can execute. Work = [", i);

                for (int j = 0; j < resource_count; j++) {
                    printf("%d", work[j]);
                    if (j < resource_count - 1) {
                        printf(" ");
                    }
                }

                printf("]\n");

                for (int j = 0; j < resource_count; j++) {
                    work[j] += processes[i].allocation[j];
                }

                processes[i].finished = true;
                safe_sequence[completed] = i;
                completed++;
                found = true;

                printf("P%d finishes and releases its resources.\n", i);
            }
        }

        if (!found) {
            break;
        }
    }

    printf("----------------------------------------\n");

    return completed == process_count;
}

int main(void) {
    int process_count;
    int resource_count;

    int available[MAX_RESOURCES];
    int safe_sequence[MAX_PROCESSES];

    Process processes[MAX_PROCESSES] = {0};

    printf("========================================\n");
    printf("   BANKER'S DEADLOCK SAFETY ALGORITHM\n");
    printf("========================================\n");

    printf("\nEnter number of processes: ");

    if (scanf("%d", &process_count) != 1 ||
        process_count < 1 || process_count > MAX_PROCESSES) {
        fprintf(stderr, "Error: Invalid number of processes.\n");
        return EXIT_FAILURE;
    }

    printf("Enter number of resource types: ");

    if (scanf("%d", &resource_count) != 1 ||
        resource_count < 1 || resource_count > MAX_RESOURCES) {
        fprintf(stderr, "Error: Invalid number of resource types.\n");
        return EXIT_FAILURE;
    }

    printf("\nEnter Allocation Matrix (%d x %d):\n",
           process_count, resource_count);

    for (int i = 0; i < process_count; i++) {
        printf("P%d: ", i);

        for (int j = 0; j < resource_count; j++) {
            if (scanf("%d", &processes[i].allocation[j]) != 1 ||
                processes[i].allocation[j] < 0) {
                fprintf(stderr, "Error: Invalid allocation value.\n");
                return EXIT_FAILURE;
            }
        }
    }

    printf("\nEnter Maximum Matrix (%d x %d):\n",
           process_count, resource_count);

    for (int i = 0; i < process_count; i++) {
        printf("P%d: ", i);

        for (int j = 0; j < resource_count; j++) {
            if (scanf("%d", &processes[i].maximum[j]) != 1 ||
                processes[i].maximum[j] < processes[i].allocation[j]) {
                fprintf(stderr,
                        "Error: Maximum demand must be at least "
                        "the allocated amount.\n");
                return EXIT_FAILURE;
            }

            processes[i].need[j] =
                processes[i].maximum[j] -
                processes[i].allocation[j];
        }
    }

    printf("\nEnter Available Resources (%d values):\n",
           resource_count);

    for (int j = 0; j < resource_count; j++) {
        if (scanf("%d", &available[j]) != 1 || available[j] < 0) {
            fprintf(stderr, "Error: Invalid available resource value.\n");
            return EXIT_FAILURE;
        }
    }

    print_matrix("Allocation Matrix:", processes,
                 process_count, resource_count, 0);

    print_matrix("Need Matrix:", processes,
                 process_count, resource_count, 1);

    printf("\nAvailable Resources: [");

    for (int j = 0; j < resource_count; j++) {
        printf("%d", available[j]);

        if (j < resource_count - 1) {
            printf(" ");
        }
    }

    printf("]\n");

    bool safe = safety_algorithm(
        processes,
        process_count,
        resource_count,
        available,
        safe_sequence
    );

    if (safe) {
        printf("\nRESULT: SYSTEM IS IN A SAFE STATE.\n");
        printf("One safe sequence is: ");

        for (int i = 0; i < process_count; i++) {
            printf("P%d", safe_sequence[i]);

            if (i < process_count - 1) {
                printf(" -> ");
            }
        }

        printf("\n");
    } else {
        printf("\nRESULT: SYSTEM IS IN AN UNSAFE STATE.\n");
        printf("No safe sequence exists for the given state.\n");
        printf("This does not automatically mean that a deadlock "
               "has already occurred.\n");
    }

    return EXIT_SUCCESS;
}
