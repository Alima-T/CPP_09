# CPP_09 - PmergeMe

PmergeMe is a C++ program that compares the sorting cost of two STL containers: `std::vector` and `std::deque`.

The project uses the Ford-Johnson merge-insert idea:

- split the input into pairs,
- keep the smaller number first and the larger number second,
- sort the pairs by the second value,
- build the main chain from all second values,
- insert the first values in Jacobsthal order,
- insert the unpaired value at the end if the input size is odd.

## What the program does

- reads positive integers from the command line,
- rejects invalid input such as letters, negatives, zero, overflow, or mixed strings,
- sorts the same input twice, once with `vector` and once with `deque`,
- prints the input, the sorted output, and the processing time for each container.

## Build

From the project directory:

```bash
make
```

## Run

```bash
./PmergeMe 18 3 7 14 2 19 9 5 11 1 16 8 4 20 6 12 17 10 15 13 77
```

## Algorithm summary

1. Split the input into pairs.
2. Inside each pair, store the smaller number first.
3. Sort the pairs by their second value.
4. Build the main chain from the second values.
5. Insert the first values in Jacobsthal order using binary search.

If the input has an odd number of values, the last value is inserted separately with `lower_bound` so the result stays sorted.

## Why Jacobsthal order

Jacobsthal order reduces the number of comparisons during insertion.

It works well because:

- the first values are not inserted in a simple left-to-right order,
- each new insertion uses `lower_bound`,
- the order of pairs follows Jacobsthal groups,
- the search range grows in a way that keeps comparisons efficient.

## Visual example

Input:

```text
18 3 7 14 2 19 9 5 11 1 16 8 4 20 6 12 17 10 15 13 77
```

This example has 21 values, so there are 10 full pairs and 1 leftover value.

### 1. Pairs and sorting by the second value

| Input pair | Normalized pair |
|---|---|
| (18, 3) | (3, 18) |
| (7, 14) | (7, 14) |
| (2, 19) | (2, 19) |
| (9, 5) | (5, 9) |
| (11, 1) | (1, 11) |
| (16, 8) | (8, 16) |
| (4, 20) | (4, 20) |
| (6, 12) | (6, 12) |
| (17, 10) | (10, 17) |
| (15, 13) | (13, 15) |

Unpaired value: `77`

Sorted by `pair.second`:

| Order | Pair |
|---|---|
| 1 | (5, 9) |
| 2 | (1, 11) |
| 3 | (6, 12) |
| 4 | (7, 14) |
| 5 | (13, 15) |
| 6 | (8, 16) |
| 7 | (10, 17) |
| 8 | (3, 18) |
| 9 | (2, 19) |
| 10 | (4, 20) |

Main chain:

```text
[9, 11, 12, 14, 15, 16, 17, 18, 19, 20]
```

### 2. Jacobsthal insertion order

For 10 pairs, the order is:

```text
1, 3, 2, 5, 4, 10, 9, 8, 7, 6
```

How this order is built:

| Step | Jacobsthal range | Added indexes |
|---|---|---|
| 1 | start | 1 |
| 2 | from 1 to 3 | 3, 2 |
| 3 | from 3 to 5 | 5, 4 |
| 4 | from 5 to 11 | 10, 9, 8, 7, 6 |

The idea is to take the next Jacobsthal boundary, then insert the indexes inside that range in reverse order.
For 10 pairs, the next boundary after 11 is bigger than the number of pairs, so the process stops at 10.

#### How the algorithm works

**Jacobsthal numbers:** 0, 1, 3, 5, 11, 21, 43, ...

Formula: `J(n) = J(n-1) + 2 × J(n-2)`

**Step 0: Initialize**

```
order = [1]               (first pair is always first)
previousJacobsthal = 1
currentJacobsthal = 3
```

**Step 1: Group from 1 to 3**

```
Is previousJacobsthal(1) < pairCount(10)?  YES
upperBound = min(3, 10) = 3
Add in reverse order from 3 down to 1 (but skip 1 itself):
  3, 2
order = [1, 3, 2]
Move to next:
  nextJacobsthal = 3 + 2×1 = 5
  previousJacobsthal = 3
  currentJacobsthal = 5
```

**Step 2: Group from 3 to 5**

```
Is previousJacobsthal(3) < pairCount(10)?  YES
upperBound = min(5, 10) = 5
Add in reverse order from 5 down to 3 (skip 3):
  5, 4
order = [1, 3, 2, 5, 4]
Move to next:
  nextJacobsthal = 5 + 2×3 = 11
  previousJacobsthal = 5
  currentJacobsthal = 11
```

**Step 3: Group from 5 to 11**

```
Is previousJacobsthal(5) < pairCount(10)?  YES
upperBound = min(11, 10) = 10  (capped at 10 because we only have 10 pairs)
Add in reverse order from 10 down to 5 (skip 5):
  10, 9, 8, 7, 6
order = [1, 3, 2, 5, 4, 10, 9, 8, 7, 6]
Move to next:
  nextJacobsthal = 11 + 2×5 = 21
  previousJacobsthal = 11
  currentJacobsthal = 21
```

**Step 4: Check if done**

```
Is previousJacobsthal(11) < pairCount(10)?  NO
Stop - all pairs are processed
```

**Final order:**

```
[1, 3, 2, 5, 4, 10, 9, 8, 7, 6]
```

The key insight: once we reach the upper boundary for a group, we don't start over—we move to the next Jacobsthal boundary and fill in the gaps in reverse order. This ensures fewer comparisons during binary search.

### 3. Insert the smaller values

| Step | Pair index | Pair | Insert value | Main chain |
|---|---:|---|---:|---|
| 1 | 1 | (5, 9) | 5 | [5, 9, 11, 12, 14, 15, 16, 17, 18, 19, 20] |
| 2 | 3 | (6, 12) | 6 | [5, 6, 9, 11, 12, 14, 15, 16, 17, 18, 19, 20] |
| 3 | 2 | (1, 11) | 1 | [1, 5, 6, 9, 11, 12, 14, 15, 16, 17, 18, 19, 20] |
| 4 | 5 | (13, 15) | 13 | [1, 5, 6, 9, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20] |
| 5 | 4 | (7, 14) | 7 | [1, 5, 6, 7, 9, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20] |
| 6 | 10 | (4, 20) | 4 | [1, 4, 5, 6, 7, 9, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20] |
| 7 | 9 | (2, 19) | 2 | [1, 2, 4, 5, 6, 7, 9, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20] |
| 8 | 8 | (3, 18) | 3 | [1, 2, 3, 4, 5, 6, 7, 9, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20] |
| 9 | 7 | (10, 17) | 10 | [1, 2, 3, 4, 5, 6, 7, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20] |
| 10 | 6 | (8, 16) | 8 | [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20] |

### 4. Insert the leftover value

The leftover value `77` is inserted at the end with `lower_bound`, so the final chain stays sorted.

| Step | Value | Main chain |
|---|---:|---|
| Final | 77 | [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 77] |

### Final result

```text
Output: 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 77
```

## Notes

- The project keeps separate implementations for `vector` and `deque`.
- The time output is shown in microseconds with decimal precision.
- The goal is to compare both containers, not only to sort the data.

## Implementation details

The algorithm uses **recursive merge sort** for sorting pairs by their second value, which follows the Ford-Johnson merge-insert definition from TAOCP Vol.3 (page 184). This ensures the implementation complies with the classical algorithm as described in Donald Knuth's work.

## Memory management

There are no memory leaks in this program because:

- **STL containers are RAII**: `std::vector` and `std::deque` automatically free their memory when they go out of scope.
- **No raw pointers**: The code doesn't use `new` or `delete` anywhere.
- **No manual memory allocation**: All data is stored in STL containers or on the stack.
- **Automatic cleanup**: When the program ends, all objects are destroyed and their memory is returned to the system.