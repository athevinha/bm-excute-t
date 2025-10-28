# Exercise 1
* `w01-byte` bytes/second: 18683.3 byte/sec
* `w02-byte` bytes/second: 237084 byte/sec
* `w03-byte` bytes/second: 13567100 byte/sec

# Exercise 2
### Scenario 1:
* How many array elements can fit into a cache block?
2 elements (Block Size = 8 bytes, each int is 4 bytes, so 8/4 = 2 elements per block)

* What combination of parameters is producing the hit rate you observe? Think about the sizes of each of the parameters.
Array Size = 128 bytes (32 elements), Block Size = 8 bytes (2 elements), Number of Blocks = 1, Step Size = 1, Rep Count = 2. With only 1 cache block and an array of 32 elements, the cache can only hold 2 elements at a time. On each pass through the array with step size 1, we access consecutive elements. Every 2 consecutive accesses give us 1 miss (load new block) + 1 hit (second element in block) = 50% hit rate per pass. Since the cache can't hold the full array and we're constantly evicting blocks, both passes have the same 50% hit rate, giving an overall hit rate of 50%.

* What is our hit rate if we increase Rep Count arbitrarily? Why?
The hit rate remains at 50% regardless of Rep Count. With only 1 cache block that holds 2 elements, but an array of 32 elements, each new pair of accesses evicts the previous block. There's no temporal locality benefit across repetitions because the cache is too small to hold the working set. Every pass through the array follows the same pattern: miss on first element of each pair, hit on the second element = 50% hit rate.

### Scenario 2:
* What combination of parameters is producing the hit rate you observe? Think about the sizes of each of the parameters.
Array Size = 128 bytes (32 elements), Step Size = 27, Block Size = 8 bytes (2 elements), Number of Blocks = 1. When stepping by 27 elements, we access elements at indices: 0, 27, 54, 81, ... Since the array has only 32 elements (indices 0-31), we only access elements 0 and 27 before exceeding the array size. Element 0 and 27 map to different cache blocks, so with only 1 cache block, accessing element 27 evicts element 0. On the second repetition (Rep Count = 2), we access 0 (miss, evicts 27), then 27 (miss, evicts 0). This gives 0% hit rate - every access is a miss because the two accessed elements keep evicting each other.

* What happens to our hit rate if we increase the number of blocks and why?
If we increase to 2 or more blocks, the hit rate improves dramatically to 50% (or higher with Rep Count > 2). With 2 blocks, both elements 0 and 27 can coexist in the cache. On the first pass: element 0 (miss), element 27 (miss). On the second pass: element 0 (hit), element 27 (hit). This gives 2 hits out of 4 total accesses = 50% hit rate. With even more repetitions, the hit rate approaches 100% because after the initial misses, all subsequent accesses hit.

### Scenario 3:
* Choose a `number of blocks` greater than `1` and determine the smallest `block size` that uses every block *and* maximizes the hit rate given the parameters above. Explain why.
Number of blocks: 16
Block Size: 8 bytes

With Array Size = 256 bytes, Step Size = 2, Rep Count = 2, and Option = 1 (read AND write, so 2 accesses per iteration):

- Step Size = 2 means we access elements at indices: 0, 2, 4, 6, 8, ... up to index 62 (since 256 bytes / 4 bytes per int = 64 elements, and we step by 2).
- This gives us 32 unique elements accessed (indices 0, 2, 4, ..., 62).
- These 32 elements = 128 bytes of data.
- With Block Size = 8 bytes, we need 128/8 = 16 blocks to hold all accessed data.
- With 16 blocks × 8 bytes = 128 bytes total cache capacity, we can fit the entire working set.
- This maximizes the hit rate because after the first pass loads all data into cache (with some spatial locality hits), the second pass hits 100% of the time.
- Block size 8 is the smallest that uses all 16 blocks while maintaining good spatial locality (each block holds 2 consecutive elements, so when we access an even-indexed element, we also load the next odd-indexed element, though we skip it due to step size 2).

# Exercise 3
* Order the functions from fastest to slowest, and explain why each function's ranking makes sense using your understanding of how the cache works. Some functions might have similar runtimes. If this is the case, explain why.

Ordering (Fastest to Slowest):
1. kji: ~0.49 Gflop/s (FASTEST)
2. jki: ~0.49 Gflop/s (FASTEST)
3. ijk: ~0.37 Gflop/s
4. jik: ~0.34 Gflop/s
5. kij: ~0.18 Gflop/s
6. ikj: ~0.17 Gflop/s (SLOWEST)

The matrices are stored in column-major order (columns are contiguous in memory). The expression is: `C[i+j*n] += A[i+k*n]*B[k+j*n]`

- kji and jki (fastest, ~0.49 Gflop/s): Both have `i` in the innermost loop. This means we're accessing consecutive elements in memory for all three matrices (A[i+...], B[...], C[i+...]). The innermost loop strides through `i`, accessing contiguous memory locations, which provides excellent spatial locality. Cache blocks are fully utilized as we read sequential elements.

- ijk and jik (moderate, ~0.35 Gflop/s): These have `k` in the innermost loop. This causes strided access patterns for matrix A (A[i+k*n] jumps by n elements each iteration), which leads to poor spatial locality. However, C is only accessed once per inner loop, and B has decent locality. The partial spatial locality gives moderate performance.

- kij and ikj (slowest, ~0.17 Gflop/s): These have `j` in the innermost loop. This causes strided access for both C and B (C[i+j*n] and B[k+j*n] jump by n elements each iteration). This results in the worst spatial locality - we're jumping across rows instead of moving along contiguous columns. Almost every access loads a new cache block with minimal reuse, leading to the highest cache miss rate and slowest performance.
