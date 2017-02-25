#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "feather.h"

extern void ft_return_cleanup(void);

// Do not change this structure without changing feather_context_switch.S as well!
struct feather_thread {
    // Callee-save registers according to cdecl
    int32_t esp;
    int32_t ebx;
    int32_t edi;
    int32_t esi;
    int32_t ebp;
    int32_t eip;
    // Upper 16 bits are the exit code, lower 16 bits 'boolean' value if already exited
    // If you change this interpretation of exited, you have to change ft_exit and feather_context_switch.S as well!
    int32_t exited;
    // Pointer to stack memory, so we can free it later
    void *stack_memory;
} __attribute__((packed));

// Do not change this structure without changing feather_context_switch.S as well!
struct feather {
    size_t thread_count;
    size_t cur_thread;
    struct feather_thread *threads;
} __attribute__((packed));

union function_ptr_void_cast {
    int32_t reg_content;
    // Generic function pointer
    void (*func_ptr)(void);
};

feather *ft_init(size_t count, __attribute__ ((unused)) ft_func *funcs) {
    struct feather *feather = calloc(1, sizeof(*feather));
    if (!feather) {
        // Preserve calloc's errno
        return NULL;
    }

    // Thread 0 represents our main thread, this allows us to simply call ft_yield(feather, 0)
    // to get the ball running in the main thread; including saving its return address.
    feather->thread_count = count + 1;
    feather->cur_thread = 0;

    feather->threads = calloc(1, sizeof(*(feather->threads)) * feather->thread_count);
    if (!feather->threads) {
        int e = errno;
        free(feather);
        errno = e;
        return NULL;
    }

    // Initialize data structures for all threads _except_ for the main thread
    for (size_t i = 0; i < count; i++) {
        char *stack = calloc(1, FT_STACK_SIZE);
        if (!stack) {
            int e = errno;
            for (size_t j = 1; j < count; j++) {
                free(feather->threads[j].stack_memory);
            }
            free(feather);
            errno = e;
            return NULL;
        }

        feather->threads[i + 1].stack_memory = stack;

        union function_ptr_void_cast func_cast;
        func_cast.func_ptr = ft_return_cleanup;

        // Lay initial parameters onto stack
        *((int32_t *)(stack + (FT_STACK_SIZE -  4))) = (int32_t) (i + 1);
        *((int32_t *)(stack + (FT_STACK_SIZE -  8))) = (int32_t) ((void *) feather);
        // Faked return address to clean up
        *((int32_t *)(stack + (FT_STACK_SIZE - 12))) = func_cast.reg_content;

        func_cast.func_ptr = (void (*)(void)) funcs[i];
        feather->threads[i + 1].eip = func_cast.reg_content;
        // Stack expands downwards, so calculate 'the end'
        feather->threads[i + 1].esp = (int32_t) ((char *) stack + FT_STACK_SIZE - 12);
    }

    return feather;
}

void ft_yieldd(struct feather *f, size_t ownId) {
    printf("Switching from %u to %u\n", ownId, (ownId + 1) % f->thread_count);
    ft_yield(f);
}

void ft_run(feather *f, int16_t *exitcodes) {
    // Yield from the main thread, includes context saving.
    ft_yield(f);

    if (exitcodes) {
        // Thread 0 is main thread
        for (size_t i = 1; i < f->thread_count; i++) {
            exitcodes[i - 1] = (int16_t) (f->threads[i].exited >> 16);
        }
    }
    ft_destroy(f);
}

void ft_destroy(feather *f) {
    for (size_t i = 1; i < f->thread_count; i++) {
        free(f->threads[i].stack_memory);
    }
    free(f->threads);
    free(f);
}

__attribute__((noreturn)) void ft_exit(feather *f, size_t ownId, int16_t exitcode) {
    f->threads[ownId].exited = (exitcode << 16) | 0x1;
    ft_yield(f);
    // To make GCC happy about noreturn attribute, won't be called anyway as ft_yield performs a context switch
    exit(EXIT_SUCCESS);
}

void ft_debug(feather *f) {
    printf("Feather (@%p) with %d, currently thread %d is running\n", (void*) f, f->thread_count, f->cur_thread);
    for (size_t i = 1; i < f->thread_count; i++) {
        printf("Thread %u\n", i);
        printf("\tesp: %p\n", (void *) f->threads[i].esp);
        printf("\tebx: %d\n", f->threads[i].ebx);
        printf("\tedi: %d\n", f->threads[i].edi);
        printf("\tesi: %d\n", f->threads[i].esi);
        printf("\tebp: %d\n", f->threads[i].ebp);
        printf("\teip: %p\n", (void *) f->threads[i].eip);
        printf("\n");
    }
}