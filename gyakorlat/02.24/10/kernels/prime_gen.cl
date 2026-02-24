__kernel void generate_primes(__global int* numbers, __global int* results, int n) {
    int gid = get_global_id(0);

    if (gid < n) {
        int num = numbers[gid];
        
        if (num < 2) {
            results[gid] = 0;
            return;
        }

        int is_prime = 1;
        for (int i = 2; i * i <= num; i++) {
            if (num % i == 0) {
                is_prime = 0;
                break;
            }
        }

        if (is_prime) {
            results[gid] = num;
        } else {
            results[gid] = 0;
        }
    }
}