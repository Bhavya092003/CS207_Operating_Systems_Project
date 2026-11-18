
#include <stdio.h>
#include <stdlib.h>

#define MAX_REFERENCES 1000
#define MAX_FRAMES 100
#define MAX_REQUESTS 1000

void print_frames(int frames[], int frame_count) {
    printf("[ ");
    for (int i = 0; i < frame_count; i++) {
        if (frames[i] == -1)
            printf("- ");
        else
            printf("%d ", frames[i]);
    }
    printf("]");
}

void fifo_page_replacement(int pages[], int n, int frame_count) {
    int frames[MAX_FRAMES];
    int next = 0, faults = 0;

    for (int i = 0; i < frame_count; i++)
        frames[i] = -1;

    printf("\n========== FIFO PAGE REPLACEMENT ==========\n");
    printf("Reference\tFrames\t\tResult\n");

    for (int i = 0; i < n; i++) {
        int found = 0;

        for (int j = 0; j < frame_count; j++) {
            if (frames[j] == pages[i]) {
                found = 1;
                break;
            }
        }

        if (found) {
            printf("%d\t\t", pages[i]);
            print_frames(frames, frame_count);
            printf("\tHIT\n");
        } else {
            frames[next] = pages[i];
            next = (next + 1) % frame_count;
            faults++;

            printf("%d\t\t", pages[i]);
            print_frames(frames, frame_count);
            printf("\tPAGE FAULT\n");
        }
    }

    printf("Total page faults (FIFO): %d\n", faults);
}

void lru_page_replacement(int pages[], int n, int frame_count) {
    int frames[MAX_FRAMES];
    int last_used[MAX_FRAMES];
    int faults = 0;

    for (int i = 0; i < frame_count; i++) {
        frames[i] = -1;
        last_used[i] = -1;
    }

    printf("\n========== LRU PAGE REPLACEMENT ==========\n");
    printf("Reference\tFrames\t\tResult\n");

    for (int i = 0; i < n; i++) {
        int found = -1;

        for (int j = 0; j < frame_count; j++) {
            if (frames[j] == pages[i]) {
                found = j;
                break;
            }
        }

        if (found != -1) {
            last_used[found] = i;

            printf("%d\t\t", pages[i]);
            print_frames(frames, frame_count);
            printf("\tHIT\n");
        } else {
            int replace = -1;

            for (int j = 0; j < frame_count; j++) {
                if (frames[j] == -1) {
                    replace = j;
                    break;
                }
            }

            if (replace == -1) {
                replace = 0;

                for (int j = 1; j < frame_count; j++) {
                    if (last_used[j] < last_used[replace])
                        replace = j;
                }
            }

            frames[replace] = pages[i];
            last_used[replace] = i;
            faults++;

            printf("%d\t\t", pages[i]);
            print_frames(frames, frame_count);
            printf("\tPAGE FAULT\n");
        }
    }

    printf("Total page faults (LRU): %d\n", faults);
}

long long distance_between(int a, int b) {
    long long difference = (long long)a - b;
    return difference < 0 ? -difference : difference;
}

void fcfs_disk_scheduling(int requests[], int n, int head) {
    long long total_movement = 0;
    int current = head;

    printf("\n========== FCFS DISK SCHEDULING ==========\n");
    printf("Initial head position: %d\n", head);
    printf("Service order: %d", head);

    for (int i = 0; i < n; i++) {
        long long movement = distance_between(current, requests[i]);
        total_movement += movement;

        printf(" -> %d (movement: %lld)\n", requests[i], movement);
        current = requests[i];
    }

    printf("Total head movement (FCFS): %lld\n", total_movement);
}

void sstf_disk_scheduling(int requests[], int n, int head) {
    int visited[MAX_REQUESTS] = {0};
    int current = head;
    long long total_movement = 0;

    printf("\n========== SSTF DISK SCHEDULING ==========\n");
    printf("Initial head position: %d\n", head);
    printf("Service order: %d", head);

    for (int count = 0; count < n; count++) {
        int nearest = -1;
        long long minimum_distance = 0;

        for (int i = 0; i < n; i++) {
            if (!visited[i]) {
                long long distance = distance_between(current, requests[i]);

                if (nearest == -1 || distance < minimum_distance) {
                    nearest = i;
                    minimum_distance = distance;
                }
            }
        }

        visited[nearest] = 1;
        total_movement += minimum_distance;

        printf(" -> %d (movement: %lld)\n",
               requests[nearest], minimum_distance);

        current = requests[nearest];
    }

    printf("Total head movement (SSTF): %lld\n", total_movement);
}

int main(void) {
    int page_count, pages[MAX_REFERENCES];
    int frame_count;
    int initial_head;
    int request_count, requests[MAX_REQUESTS];

    if (scanf("%d", &page_count) != 1 ||
        page_count <= 0 || page_count > MAX_REFERENCES) {
        fprintf(stderr, "Invalid page reference count.\n");
        return 1;
    }

    for (int i = 0; i < page_count; i++) {
        if (scanf("%d", &pages[i]) != 1 || pages[i] < 0) {
            fprintf(stderr, "Invalid page reference.\n");
            return 1;
        }
    }

    if (scanf("%d", &frame_count) != 1 ||
        frame_count <= 0 || frame_count > MAX_FRAMES) {
        fprintf(stderr, "Invalid frame count.\n");
        return 1;
    }

    if (scanf("%d", &initial_head) != 1 || initial_head < 0) {
        fprintf(stderr, "Invalid initial disk head position.\n");
        return 1;
    }

    if (scanf("%d", &request_count) != 1 ||
        request_count <= 0 || request_count > MAX_REQUESTS) {
        fprintf(stderr, "Invalid disk request count.\n");
        return 1;
    }

    for (int i = 0; i < request_count; i++) {
        if (scanf("%d", &requests[i]) != 1 || requests[i] < 0) {
            fprintf(stderr, "Invalid disk request.\n");
            return 1;
        }
    }

    fifo_page_replacement(pages, page_count, frame_count);
    lru_page_replacement(pages, page_count, frame_count);
    fcfs_disk_scheduling(requests, request_count, initial_head);
    sstf_disk_scheduling(requests, request_count, initial_head);

    return 0;
}
