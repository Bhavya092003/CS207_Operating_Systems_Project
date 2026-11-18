
# Part E — Memory Management and Disk Scheduling Experiments

## 1. Objective

The objective of Part E is to implement and compare two page replacement algorithms and two disk scheduling algorithms using the C programming language.

The algorithms implemented are:

1. FIFO (First-In First-Out) Page Replacement
2. LRU (Least Recently Used) Page Replacement
3. FCFS (First Come First Served) Disk Scheduling
4. SSTF (Shortest Seek Time First) Disk Scheduling

The program displays intermediate execution steps and calculates the total page faults and disk head movement.

## 2. Technologies Used

- Programming Language: C
- Standard: C17
- Compiler: GCC
- Build System: GNU Make
- Environment: Ubuntu 24.04 on WSL

## 3. FIFO Page Replacement

FIFO replaces the page that entered memory first.

### Working

1. Initially, all memory frames are empty.
2. When a page is referenced, the program checks whether it is already present.
3. If present, it is a page hit.
4. Otherwise, a page fault occurs.
5. The new page is inserted into the next FIFO position.
6. The replacement position moves circularly through the frames.

### Test Results

| Test Case | Number of Frames | Page Faults |
|---|---:|---:|
| Test 1 | 3 | 10 |
| Test 2 | 4 | 10 |

## 4. LRU Page Replacement

LRU replaces the page that has not been used for the longest time.

### Working

1. Initially, all memory frames are empty.
2. The program checks whether the referenced page is already present.
3. If present, it updates the page's last-used timestamp.
4. If absent, a page fault occurs.
5. An empty frame is used if available.
6. Otherwise, the page with the oldest last-used timestamp is replaced.

### Test Results

| Test Case | Number of Frames | Page Faults |
|---|---:|---:|
| Test 1 | 3 | 9 |
| Test 2 | 4 | 8 |

## 5. FCFS Disk Scheduling

FCFS services disk requests in the order in which they arrive.

### Working

1. The initial disk head position is taken as input.
2. Requests are processed in their original order.
3. The movement for each request is calculated using:

   Movement = |Current Head Position - Requested Position|

4. All movements are added to calculate total head movement.

### Test Results

| Test Case | Initial Head | Total Head Movement |
|---|---:|---:|
| Test 1 | 53 | 640 |
| Test 2 | 100 | 240 |

## 6. SSTF Disk Scheduling

SSTF selects the pending disk request closest to the current head position.

### Working

1. The initial disk head position is taken as input.
2. The program calculates the distance between the current head and every unserviced request.
3. The nearest request is selected.
4. The request is marked as serviced.
5. The head moves to the selected request.
6. The process repeats until all requests are serviced.

If two requests have equal distances, the program selects the one appearing earlier in the original request queue.

### Test Results

| Test Case | Initial Head | Total Head Movement |
|---|---:|---:|
| Test 1 | 53 | 236 |
| Test 2 | 100 | 224 |

## 7. Test Input Details

### Test Case 1

**Page Replacement**

- Reference string: 7, 0, 1, 2, 0, 3, 0, 4, 2, 3, 0, 3, 2
- Number of frames: 3

**Disk Scheduling**

- Initial head position: 53
- Request queue: 98, 183, 37, 122, 14, 124, 65, 67

### Test Case 2

**Page Replacement**

- Reference string: 1, 2, 3, 4, 1, 2, 5, 1, 2, 3, 4, 5
- Number of frames: 4

**Disk Scheduling**

- Initial head position: 100
- Request queue: 55, 58, 39, 18, 90, 160, 150

The program displays the frame contents after every page reference and the individual head movements for every disk request. Complete execution logs are stored in the outputs/part_e directory.

## 8. Comparative Analysis

### Page Replacement

| Algorithm | Test 1 Faults | Test 2 Faults |
|---|---:|---:|
| FIFO | 10 | 10 |
| LRU | 9 | 8 |

LRU produced fewer page faults in both test cases because it considers recent page usage, whereas FIFO considers the order in which pages entered memory.

However, LRU requires tracking recent usage information, which introduces additional overhead. FIFO is simpler to implement.

### Disk Scheduling

| Algorithm | Test 1 Movement | Test 2 Movement |
|---|---:|---:|
| FCFS | 640 | 240 |
| SSTF | 236 | 224 |

SSTF produced lower total head movement in both test cases because it prioritizes nearby requests.

FCFS is simpler and preserves request arrival order, providing predictable fairness.

SSTF may cause starvation because requests located far from the current head position can be postponed repeatedly when closer requests continue arriving.

## 9. Why Lower Values Are Not Always Optimal

An algorithm should not be judged only by page faults or total disk head movement.

- LRU requires additional bookkeeping to track page usage.
- FIFO is simpler but may replace a frequently used page.
- SSTF reduces head movement but may cause starvation.
- FCFS preserves request order but can result in greater head movement.

Therefore, algorithm selection depends on performance, fairness, implementation complexity, and system requirements.

## 10. Compilation and Execution

Compile the program:

```bash
make part-e
```

Run using the first test case:

```bash
make run-part-e
```

Run all automated tests:

```bash
make test-part-e
```

The automated test script verifies the expected results for both test cases.

## 11. Conclusion

The experiment successfully implemented FIFO and LRU page replacement algorithms along with FCFS and SSTF disk scheduling algorithms.

Both test cases demonstrated differences in algorithm performance. LRU generated fewer page faults than FIFO, while SSTF resulted in lower total disk head movement than FCFS.

The experiment illustrates the importance of selecting operating system algorithms based on multiple performance considerations rather than a single metric.
