// 1. Every work unit checks a single divisor
__kernel void check_single_divisor(long number, __global int* found_divisor) {
    long i = get_global_id(0);
    long divisor = 2 + i;

    // Only check up to sqrt(number)
    if (divisor * divisor <= number) {
        if (number % divisor == 0) {
            *found_divisor = 1; // Mark as not prime
        }
    }
}

// 2. Every work unit checks a range of divisors
__kernel void check_range_divisors(long number, int range_size, __global int* found_divisor) {
    long gid = get_global_id(0);
    long start = 2 + (gid * range_size);
    long end = start + range_size;

    for (long i = start; i < end; i++) {
        if (i * i <= number) {
            if (number % i == 0) {
                *found_divisor = 1;
                break;
            }
        } else {
            break;
        }
    }
}