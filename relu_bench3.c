#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#ifndef N
#define N 20000000
#endif

#ifndef REPEAT
#define REPEAT 30
#endif

__attribute__((noinline))
static int32_t relu_branch(int32_t x)
{
    int32_t out;
    asm volatile(
        "test %[x], %[x]\n\t"
        "js 1f\n\t"
        "mov %[x], %[out]\n\t"
        "jmp 2f\n\t"
        "1:\n\t"
        "xor %[out], %[out]\n\t"
        "2:\n\t"
        : [out] "=&r"(out)
        : [x] "r"(x)
        : "cc"
    );
    return out;
}

__attribute__((noinline))
static int32_t relu_branchless(int32_t x)
{
    uint32_t sign = ((uint32_t)x) >> 31;
    uint32_t mask = sign - 1;
    return (int32_t)(((uint32_t)x) & mask);
}

static void fill_data(int32_t *a, size_t n, const char *mode)
{
    size_t i;

    if (strcmp(mode, "mostly_pos") == 0) {
        for (i = 0; i < n; i++) {
            int r = rand() % 100;
            if (r < 95)
                a[i] = rand() & 0x7fffffff;
            else
                a[i] = -((int32_t)(rand() & 0x7fffffff) + 1);
        }
    } else if (strcmp(mode, "mostly_neg") == 0) {
        for (i = 0; i < n; i++) {
            int r = rand() % 100;
            if (r < 95)
                a[i] = -((int32_t)(rand() & 0x7fffffff) + 1);
            else
                a[i] = rand() & 0x7fffffff;
        }
    } else if (strcmp(mode, "random50") == 0) {
        for (i = 0; i < n; i++) {
            int32_t v = (int32_t)rand();
            if (rand() & 1)
                a[i] = v;
            else
                a[i] = -v;
        }
    } else {
        fprintf(stderr, "Unknown mode: %s\n", mode);
        exit(1);
    }
}

int main(int argc, char **argv)
{
    if (argc != 3) {
        fprintf(stderr, "Usage: %s [branch|branchless] [mostly_pos|mostly_neg|random50]\n", argv[0]);
        return 1;
    }

    const char *impl = argv[1];
    const char *mode = argv[2];

    int32_t *a = aligned_alloc(64, N * sizeof(int32_t));
    if (!a) {
        perror("aligned_alloc");
        return 1;
    }

    srand(0);
    fill_data(a, N, mode);

    volatile int64_t sink = 0;
    size_t r, i;

    if (strcmp(impl, "branch") == 0) {
        for (r = 0; r < REPEAT; r++) {
            for (i = 0; i < N; i++)
                sink += relu_branch(a[i]);
        }
    } else if (strcmp(impl, "branchless") == 0) {
        for (r = 0; r < REPEAT; r++) {
            for (i = 0; i < N; i++)
                sink += relu_branchless(a[i]);
        }
    } else {
        fprintf(stderr, "Unknown impl: %s\n", impl);
        free(a);
        return 1;
    }

    printf("sink=%lld\n", (long long)sink);
    free(a);
    return 0;
}
