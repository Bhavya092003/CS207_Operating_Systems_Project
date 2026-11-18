#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_CAPACITY 5
#define PRODUCER_COUNT 2
#define CONSUMER_COUNT 2
#define DEFAULT_ITEM_COUNT 100
#define MIN_ITEM_COUNT 100
#define MAX_ITEM_COUNT 1000000

typedef struct {
    int id;
    int producer_id;
    bool is_stop_item;
} Item;

typedef struct {
    Item items[BUFFER_CAPACITY];
    size_t in;
    size_t out;
    size_t count;
    sem_t empty_slots;
    sem_t full_slots;
    pthread_mutex_t mutex;
    int total_items;
    int *produced_by;
    int *consumed_by;
    int produced_count;
    int consumed_count;
    int duplicate_productions;
    int duplicate_consumptions;
    int invalid_items;
} SharedBuffer;

typedef struct {
    SharedBuffer *shared;
    int id;
    int total_items;
} ThreadArgs;

static void fail_system_call(const char *operation) {
    perror(operation);
    exit(EXIT_FAILURE);
}

static void fail_pthread_call(const char *operation, int error_code) {
    fprintf(stderr, "%s: %s\n", operation, strerror(error_code));
    exit(EXIT_FAILURE);
}

static void wait_semaphore(sem_t *semaphore) {
    int result;

    do {
        result = sem_wait(semaphore);
    } while (result == -1 && errno == EINTR);

    if (result == -1) {
        fail_system_call("sem_wait");
    }
}

static void post_semaphore(sem_t *semaphore) {
    if (sem_post(semaphore) == -1) {
        fail_system_call("sem_post");
    }
}

static void lock_buffer(SharedBuffer *shared) {
    int result = pthread_mutex_lock(&shared->mutex);

    if (result != 0) {
        fail_pthread_call("pthread_mutex_lock", result);
    }
}

static void unlock_buffer(SharedBuffer *shared) {
    int result = pthread_mutex_unlock(&shared->mutex);

    if (result != 0) {
        fail_pthread_call("pthread_mutex_unlock", result);
    }
}

static void enqueue_item(SharedBuffer *shared, Item item) {
    wait_semaphore(&shared->empty_slots);
    lock_buffer(shared);

    if (shared->count >= BUFFER_CAPACITY || shared->in >= BUFFER_CAPACITY) {
        fputs("Fatal error: bounded buffer overflow prevented.\n", stderr);
        abort();
    }

    shared->items[shared->in] = item;
    shared->in = (shared->in + 1U) % BUFFER_CAPACITY;
    shared->count++;

    if (item.is_stop_item) {
        printf("Main queued a shutdown item (buffer occupancy: %zu/%d)\n",
               shared->count, BUFFER_CAPACITY);
    } else {
        if (item.id < 1 || item.id > shared->total_items) {
            shared->invalid_items++;
        } else if (shared->produced_by[item.id] != 0) {
            shared->duplicate_productions++;
        } else {
            shared->produced_by[item.id] = item.producer_id;
        }

        shared->produced_count++;
        printf("Producer %d produced item %d (buffer occupancy: %zu/%d)\n",
               item.producer_id, item.id, shared->count, BUFFER_CAPACITY);
    }

    unlock_buffer(shared);
    post_semaphore(&shared->full_slots);
}

static void *producer_thread(void *argument) {
    ThreadArgs *args = argument;

    for (int item_id = args->id; item_id <= args->total_items;
         item_id += PRODUCER_COUNT) {
        Item item = {
            .id = item_id,
            .producer_id = args->id,
            .is_stop_item = false,
        };

        enqueue_item(args->shared, item);
    }

    return NULL;
}

static void *consumer_thread(void *argument) {
    ThreadArgs *args = argument;
    SharedBuffer *shared = args->shared;

    for (;;) {
        Item item;

        wait_semaphore(&shared->full_slots);
        lock_buffer(shared);

        if (shared->count == 0 || shared->out >= BUFFER_CAPACITY) {
            fputs("Fatal error: bounded buffer underflow prevented.\n", stderr);
            abort();
        }

        item = shared->items[shared->out];
        shared->out = (shared->out + 1U) % BUFFER_CAPACITY;
        shared->count--;

        if (item.is_stop_item) {
            printf("Consumer %d received a shutdown item\n", args->id);
            unlock_buffer(shared);
            post_semaphore(&shared->empty_slots);
            return NULL;
        }

        if (item.id < 1 || item.id > shared->total_items ||
            shared->produced_by[item.id] == 0) {
            shared->invalid_items++;
        } else if (shared->consumed_by[item.id] != 0) {
            shared->duplicate_consumptions++;
        } else {
            shared->consumed_by[item.id] = args->id;
            shared->consumed_count++;
        }

        printf("Consumer %d consumed item %d from Producer %d (buffer occupancy: %zu/%d)\n",
               args->id, item.id, item.producer_id, shared->count,
               BUFFER_CAPACITY);

        unlock_buffer(shared);
        post_semaphore(&shared->empty_slots);
    }
}

static bool verify_results(const SharedBuffer *shared) {
    bool passed = true;

    if (shared->produced_count != shared->total_items) {
        passed = false;
    }

    if (shared->consumed_count != shared->total_items) {
        passed = false;
    }

    if (shared->duplicate_productions != 0 ||
        shared->duplicate_consumptions != 0 || shared->invalid_items != 0 ||
        shared->count != 0) {
        passed = false;
    }

    for (int item_id = 1; item_id <= shared->total_items; item_id++) {
        if (shared->produced_by[item_id] == 0 ||
            shared->consumed_by[item_id] == 0) {
            passed = false;
            break;
        }
    }

    printf("\nSummary: produced=%d, consumed=%d, duplicate productions=%d, "
           "duplicate consumptions=%d, invalid items=%d, final buffer "
           "occupancy=%zu\n",
           shared->produced_count, shared->consumed_count,
           shared->duplicate_productions, shared->duplicate_consumptions,
           shared->invalid_items, shared->count);

    if (passed) {
        printf("Verification: PASS - produced %d items and consumed each exactly once.\n",
               shared->total_items);
        printf("Buffer bounds: PASS - capacity %d, no overflow or underflow.\n",
               BUFFER_CAPACITY);
    } else {
        puts("Verification: FAIL - one or more item or buffer invariants were violated.");
    }

    return passed;
}

static bool parse_item_count(int argc, char **argv, int *item_count) {
    char *end = NULL;
    long value;

    if (argc == 1) {
        *item_count = DEFAULT_ITEM_COUNT;
        return true;
    }

    if (argc != 2) {
        fprintf(stderr, "Usage: %s [item_count]\n", argv[0]);
        return false;
    }

    errno = 0;
    value = strtol(argv[1], &end, 10);

    if (errno == ERANGE || end == argv[1] || *end != '\0' ||
        value < MIN_ITEM_COUNT || value > MAX_ITEM_COUNT) {
        fprintf(stderr,
                "Invalid item count '%s'. Enter an integer from %d to %d.\n",
                argv[1], MIN_ITEM_COUNT, MAX_ITEM_COUNT);
        return false;
    }

    *item_count = (int)value;
    return true;
}

int main(int argc, char **argv) {
    SharedBuffer shared = {0};
    pthread_t producers[PRODUCER_COUNT];
    pthread_t consumers[CONSUMER_COUNT];
    ThreadArgs producer_args[PRODUCER_COUNT];
    ThreadArgs consumer_args[CONSUMER_COUNT];
    int item_count;
    int result;

    if (!parse_item_count(argc, argv, &item_count)) {
        return EXIT_FAILURE;
    }

    setvbuf(stdout, NULL, _IOLBF, 0);

    shared.total_items = item_count;
    shared.produced_by = calloc((size_t)item_count + 1U,
                                sizeof *shared.produced_by);
    shared.consumed_by = calloc((size_t)item_count + 1U,
                                sizeof *shared.consumed_by);

    if (shared.produced_by == NULL || shared.consumed_by == NULL) {
        fputs("Unable to allocate item verification tables.\n", stderr);
        free(shared.produced_by);
        free(shared.consumed_by);
        return EXIT_FAILURE;
    }

    result = pthread_mutex_init(&shared.mutex, NULL);
    if (result != 0) {
        fail_pthread_call("pthread_mutex_init", result);
    }

    if (sem_init(&shared.empty_slots, 0, BUFFER_CAPACITY) == -1) {
        fail_system_call("sem_init(empty_slots)");
    }

    if (sem_init(&shared.full_slots, 0, 0) == -1) {
        fail_system_call("sem_init(full_slots)");
    }

    printf("Producer-Consumer simulation: %d producers, %d consumers, "
           "buffer capacity %d, items %d\n\n",
           PRODUCER_COUNT, CONSUMER_COUNT, BUFFER_CAPACITY, item_count);

    for (int i = 0; i < CONSUMER_COUNT; i++) {
        consumer_args[i] = (ThreadArgs){
            .shared = &shared,
            .id = i + 1,
            .total_items = item_count,
        };

        result = pthread_create(&consumers[i], NULL, consumer_thread,
                                &consumer_args[i]);
        if (result != 0) {
            fail_pthread_call("pthread_create(consumer)", result);
        }
    }

    for (int i = 0; i < PRODUCER_COUNT; i++) {
        producer_args[i] = (ThreadArgs){
            .shared = &shared,
            .id = i + 1,
            .total_items = item_count,
        };

        result = pthread_create(&producers[i], NULL, producer_thread,
                                &producer_args[i]);
        if (result != 0) {
            fail_pthread_call("pthread_create(producer)", result);
        }
    }

    for (int i = 0; i < PRODUCER_COUNT; i++) {
        result = pthread_join(producers[i], NULL);
        if (result != 0) {
            fail_pthread_call("pthread_join(producer)", result);
        }
    }

    for (int i = 0; i < CONSUMER_COUNT; i++) {
        Item stop_item = {
            .id = 0,
            .producer_id = 0,
            .is_stop_item = true,
        };

        enqueue_item(&shared, stop_item);
    }

    for (int i = 0; i < CONSUMER_COUNT; i++) {
        result = pthread_join(consumers[i], NULL);
        if (result != 0) {
            fail_pthread_call("pthread_join(consumer)", result);
        }
    }

    bool passed = verify_results(&shared);

    if (sem_destroy(&shared.empty_slots) == -1) {
        fail_system_call("sem_destroy(empty_slots)");
    }
    if (sem_destroy(&shared.full_slots) == -1) {
        fail_system_call("sem_destroy(full_slots)");
    }

    result = pthread_mutex_destroy(&shared.mutex);
    if (result != 0) {
        fail_pthread_call("pthread_mutex_destroy", result);
    }

    free(shared.produced_by);
    free(shared.consumed_by);

    return passed ? EXIT_SUCCESS : EXIT_FAILURE;
}
