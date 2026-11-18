# Part C — POSIX Threads and Producer–Consumer Synchronization

## Requirements covered

- Two producer threads and two consumer threads.
- A bounded circular buffer with capacity 5.
- POSIX threads (`pthread_create` and `pthread_join`).
- A pthread mutex and two POSIX unnamed semaphores.
- Unique item IDs, with producer and consumer IDs logged for every item.
- A default stress run of 100 items; larger runs are supported up to 1,000,000 items.
- A run-time audit that checks every item was produced and consumed exactly once.
- Explicit checks that the buffer never exceeds its capacity or is read while empty.

## Build and run

From the repository root, after adding `include Makefile.part_c` to the existing Makefile:

```bash
make part-c
./bin/producer_consumer
./bin/producer_consumer 250
```

The default run processes 100 items. The optional argument must be an integer from 100 through 1,000,000.

Run the Part C tests:

```bash
make test-part-c
```

Run both Part B and Part C test suites after the Makefile integration:

```bash
make test
```

Save a reproducible execution log:

```bash
mkdir -p outputs screenshots/part_c
./bin/producer_consumer 100 | tee outputs/part_c-producer-consumer.txt
```

## Design

The shared buffer is a circular queue. `in` identifies the next free slot, `out` identifies the next occupied slot to remove, and `count` records the number of items currently stored. Both indices wrap around using modulo 5.

There are exactly two producer threads and two consumer threads. Producer 1 creates item IDs 1, 3, 5, and so on; Producer 2 creates IDs 2, 4, 6, and so on. This assigns every ID from 1 through the requested item count to exactly one producer without needing an additional shared work counter.

After both producers finish, the main thread inserts two special shutdown items. The consumers process the ordinary items first because the queue is FIFO, then each consumer exits when it removes a shutdown item. Shutdown items are control messages and are not included in the item totals.

## Critical section

The critical section is the part of `enqueue_item` and `consumer_thread` protected by `pthread_mutex_lock` and `pthread_mutex_unlock`. It includes reading or writing the shared buffer slot, updating the circular-buffer index and occupancy count, updating the verification tables and counters, and printing the corresponding operation log. Only one thread can execute these shared-state operations at a time.

## Synchronization primitives

- `empty_slots` is a counting semaphore initialized to 5. A producer must successfully decrement it before writing, so a producer waits when the buffer is full.
- `full_slots` is a counting semaphore initialized to 0. A consumer must successfully decrement it before reading, so a consumer waits when the buffer is empty.
- `mutex` is a pthread mutex initialized once. It protects the buffer indices, occupancy, audit tables, counters, and output log from simultaneous updates.

The order is important. A producer waits on `empty_slots`, enters the mutex-protected critical section, inserts one item, leaves the critical section, and posts `full_slots`. A consumer waits on `full_slots`, enters the critical section, removes one item, leaves the critical section, and posts `empty_slots`. The mutex is not held while waiting on a semaphore.

## Core operating-system concepts

**Critical section:** Code that accesses shared data and must not be executed concurrently by multiple threads. Here, it is the code that updates the bounded buffer and its associated state.

**Race condition:** A result that depends on unpredictable thread timing. Without synchronization, two producers could select the same buffer slot or two consumers could remove the same item.

**Mutual exclusion:** The guarantee that only one thread at a time enters the protected critical section. The pthread mutex provides this guarantee for shared buffer state.

**Semaphore:** A synchronization counter used to coordinate access to a limited number of resources. The two counting semaphores track available and occupied buffer slots.

## Deliberately incorrect design and why it fails

The following pseudocode is intentionally unsafe:

```text
Producer:
    if count < BUFFER_CAPACITY:
        buffer[in] = item
        in = (in + 1) % BUFFER_CAPACITY
        count = count + 1

Consumer:
    if count > 0:
        item = buffer[out]
        out = (out + 1) % BUFFER_CAPACITY
        count = count - 1
```

There is no mutex or semaphore. Two producers can both read the same `count` and `in` before either writes. They can overwrite the same slot and both increment `count`, so the count no longer describes the actual contents. Similarly, two consumers can both observe `count > 0`, read the same slot, and decrement the count twice. The result can be lost items, duplicate consumption, incorrect indices, buffer overflow, or buffer underflow.

Even using the two semaphores without a mutex is insufficient: the semaphores reserve a slot but do not make the multi-step updates to `in`, `out`, `count`, and the buffer array mutually exclusive. The mutex is still required.

## Verification and test evidence

The program maintains `produced_by[id]` and `consumed_by[id]` tables. These are updated only while holding the mutex. After all producer and consumer threads have joined, the main thread checks:

1. The number of produced ordinary items equals the requested total.
2. The number of consumed ordinary items equals the requested total.
3. Every ID from 1 through the total appears in both verification tables.
4. No ID was produced or consumed more than once.
5. No invalid item was observed and the final buffer occupancy is zero.

A successful run ends with lines similar to:

```text
Summary: produced=100, consumed=100, duplicate productions=0, duplicate consumptions=0, invalid items=0, final buffer occupancy=0
Verification: PASS - produced 100 items and consumed each exactly once.
Buffer bounds: PASS - capacity 5, no overflow or underflow.
```

The exact interleaving of producer and consumer log lines can vary between runs because thread scheduling is nondeterministic. The counts and verification result should remain the same.

Automated test cases cover the default 100-item stress run, a 1,000-item stress run, and invalid command-line values.

## Limitations

This is a bounded-buffer synchronization demonstration, not a throughput benchmark. The mutex is held while an operation is logged so that output lines remain readable and attributable; console I/O therefore adds time to the critical section. Producer workloads are split by alternating item IDs. The program uses two producers, two consumers, and a fixed buffer capacity of five, as required by the assignment.
