
# Part D - Deadlock Detection and Avoidance

## 1. Objective

The objective of this experiment is to implement the Banker's Safety Algorithm in C to analyse resource allocation and determine whether a system is in a safe or unsafe state.

The program accepts the number of processes, resource types, Allocation matrix, Maximum matrix and Available resources.

## 2. Algorithm Used

Banker's Algorithm is a deadlock avoidance algorithm.

It checks whether the system can allocate resources to processes in some order such that every process can finish successfully.

### Need Matrix

Need represents the additional resources required by each process.

Need[i][j] = Maximum[i][j] - Allocation[i][j]

### Safety Algorithm

1. Initialize Work = Available.
2. Mark all processes as unfinished.
3. Find an unfinished process whose Need is less than or equal to Work for every resource type.
4. Assume that process completes and releases its allocated resources.
5. Update Work = Work + Allocation.
6. Mark the process as finished and add it to the safe sequence.
7. Repeat until all processes finish or no eligible process can be found.

If all processes finish, the system is safe. Otherwise, it is unsafe.

## 3. Implementation

Source file: src/part_d/banker.c

The implementation uses:
- C17 standard.
- Two-dimensional arrays for Allocation, Maximum and Need.
- A Work array to track temporarily available resources.
- A Finish flag for every process.
- A safe sequence array to store process execution order.

The program also validates input dimensions, non-negative resource values and maximum demands.

## 4. Test Cases and Results

### Test Case 1: Safe State

Input: 5 processes and 3 resource types.

Available = [3, 3, 2]

Expected safe sequence:

P1 -> P3 -> P4 -> P0 -> P2

Result: System is in a safe state.

### Test Case 2: Unsafe State

Input: 2 processes and 1 resource type.

Available = [0]

Neither process can obtain the additional resources required to complete.

Result: No safe sequence exists.

An unsafe state does not necessarily mean that deadlock has already occurred.

### Test Case 3: Multiple Safe Sequences

Input: 3 processes and 2 resource types.

Available = [1, 1]

Each process has a remaining need of [1, 1]. Therefore, more than one process can execute first.

One valid safe sequence:

P0 -> P1 -> P2

Result: System is in a safe state.

## 5. Four Necessary Conditions for Deadlock

Deadlock requires all four conditions to hold simultaneously.

1. Mutual Exclusion:
   At least one resource must be held in a non-shareable manner.

2. Hold and Wait:
   A process holds allocated resources while waiting for additional resources.

3. No Preemption:
   Resources cannot be forcibly taken away from a process.

4. Circular Wait:
   A circular chain of processes exists, where each process waits for a resource held by the next process.

## 6. Deadlock Handling Techniques

### Prevention
Prevents deadlock by ensuring that at least one of the four necessary conditions cannot hold.

### Avoidance
Examines resource requests before granting them and ensures that the system remains in a safe state. Banker's Algorithm is an example.

### Detection
Allows deadlocks to occur and periodically checks the resource allocation state to identify them.

### Recovery
Takes corrective action after deadlock detection, such as terminating processes or preempting resources where possible.

## 7. Compilation and Execution

Compile:

make part-d

Run:

./bin/banker

Run automated tests:

make test-part-d

Individual test inputs:

tests/part_d/safe.txt
tests/part_d/unsafe.txt
tests/part_d/multiple_safe.txt

## 8. Output Files

Execution outputs are saved in:

outputs/part_d/

- safe_output.txt
- unsafe_output.txt
- multiple_safe_output.txt

## 9. Conclusion

The Banker's Safety Algorithm was successfully implemented and tested.

The program correctly identifies safe and unsafe states and displays a valid safe sequence when one exists.

All three test cases passed, demonstrating the correctness of the implementation for the tested scenarios.

