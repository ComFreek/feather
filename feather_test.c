#include <stdlib.h>
#include <stdio.h>
#include "feather.h"

// Feather-aware Bounded Buffer with 'blocking' (= yielding) get and put operations
// for producer-consumer situations.
typedef struct BNDBUF {
    // Only the producer (consumer) will access in_idx (out_idx) => volatile is unnecessary in both cases
    size_t in_idx;
    size_t out_idx;
    volatile size_t free; // Free elements
    size_t size;
    feather *feather;
    volatile int buffer[];
} BNDBUF;

static BNDBUF *bb_init(size_t size, feather *feather) {
    // Beware, overflows in size computation possible
    BNDBUF *bb = calloc(1, sizeof(*bb) + (sizeof(*bb->buffer) * size));
    if (!bb) {
        return NULL;
    }
    bb->size = bb->free = size;
    bb->feather = feather;
    return bb;
}

static void bb_destroy(BNDBUF *bb) {
    free(bb);
}

static int bb_get(BNDBUF *bb) {
    while (bb->free == bb->size) ft_yield((feather *) bb->feather);

    return (bb->free++, bb->buffer[bb->out_idx++ % bb->size]);
}

static void bb_put(BNDBUF *bb, int value) {
    while (bb->free == 0) ft_yield((feather *) bb->feather);

    (bb->free--, bb->buffer[bb->in_idx++ % bb->size] = value);
}

static BNDBUF *bb;
static int MAX_PRIME_NUMBER = 30;

static int16_t producer(struct feather* feather, size_t own_id) {
    (void) feather;
    (void) own_id;

    for (int n = 2; n < MAX_PRIME_NUMBER; n++) {
        int prime = 1;
        for (int i = 2; i < n; i++) {
            if (n % i == 0) {
                prime = 0;
                break;
            }
        }
        if (prime) {
            bb_put(bb, n);
        }
    }

    // Deadpill
    bb_put(bb, -1);

    return 0;
}

static int16_t consumer(struct feather* feather, size_t own_id) {
    (void) feather;
    (void) own_id;

    int prime_number;
    while ((prime_number = bb_get(bb)) != -1) {
        printf("New prime number: %d\n", prime_number);
    }

    return 0;
}

int main(__attribute__ ((unused)) int argc, __attribute__ ((unused)) char *argv[]) {
    ft_func funcs[2] = {producer, consumer};
    struct feather *f = ft_init(2, funcs);
    if (!f) {
        perror("ft_init");
        return EXIT_FAILURE;
    }

    bb = bb_init(10, f);
    if (!bb) {
        perror("BNDBUF init");
        ft_destroy(f);
        return EXIT_FAILURE;
    }

    ft_run(f, NULL);
    
    bb_destroy(bb);
}
