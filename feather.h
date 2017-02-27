#ifndef FEATHER_H
#define FEATHER_H

#if !defined(__GNUC__) && !defined(__GNUG__)
  #error "Feather is only supported with GCC"
#endif

// To prevent hard-to-find errors, we simply stop compilation if SSE is enabled.
#if defined(__SSE__) || defined(__SSE2__) || defined(__SSE_MATH__) || defined(__SSE2_MATH__)
  #error "Feather does not work with SSE as it does not save these registers between context switches. See comments in feather.h"
#endif

#if defined(__x86_64__) || defined(_M_X64)
  #error "Feather does not work with x86-64 (IA-64 or other names) as it only saves x86 regsiters between context switches."
#endif

#if !defined(__i386) && !defined(_M_IX86)
  #error "Feather does not work on non-x86 architectures."
#endif

/**
  * # Feather
  * An x86 (IA-32) user-level threads implementation. Cooperative. FCFS strategy. Thread-safe.
  *
  * The user-level threads act cooperatively, i.e. every thread must voluntarily give up its runtime by either
  * calling ft\_yield, ft\_exit or returning from its entry point (equivalent to calling ft\_exit).
  * The scheduler, as implemented in ft_yield, will select the next (as in the one with the incremented index)
  * thread which has not yet exited.
  * If all threads have exited, ft_run writes their exit codes into the given parameter (if not NULL) and returns.
  *
  * This implementation only shares state by means of the feather* pointers passed across various functions and
  * the usage of malloc/free. Assuming malloc/free are thread-safe, you can safely run user-level threads from
  * _different_ initialized feather\* structures from multiple real threads. Note that user-level threads across real
  * threads can also share state and communicate among themselves.
  *
  * ## Compatibility Notes
  *
  *  1. Currently the only supported compiler is GCC:
  *     It would most likely work on other compilers with minor modifications,
  *     e.g. \_\_attribute\_\_ ((…)) directives would need to be ported.
  *  2. Only works on x86 (IA-32, 32-bit)
  *  3. The user-level threads must not make use of SSE instructions or SSE registers as these are
  *     not taken into account when context-switching.
  *
  * ## Trivia
  * Its name originates from the German word 'Feder' which is sometimes used to refer to user-level-threads
  * ("federgewichtige Threads").
  */

typedef struct feather_thread feather_thread;
typedef struct feather feather;

/**
  * User-level thread entry point.
  */
typedef __attribute__((cdecl)) int16_t (*ft_func)(struct feather* feather, size_t ownId);

/**
  * The default stack size in bytes.
  */
static const size_t FT_STACK_SIZE = 4096;

/**
  * Initializes, but not yet runs the threads.
  * Each threads is given its own stack with size FT_STACK_SIZE on the heap, i.e. allocated via malloc.
  * The stack and all other data structures as referenced by
  *
  * @return On succes, a non-NULL pointer is returned. On failure, NULL is returned and errno set to indicate the
  *         error.
  */
feather *ft_init(size_t count, ft_func *funcs);

/**
  * Wrapper function for ft_init which takes the functions as parameters instead of as an array.
  */
feather *ft_initv(size_t count, ...);

/**
  * Equivalent to returning from the thread's entry point function except that you can
  * everywhere call this function within the thread (e.g. if a helper function called by the entry point
  * function determines that forceful termination is required).
  *
  * The parameters feather and ownId need to match those which where passed to the thread entry point function.
  */
__attribute__((noreturn)) void ft_exit(feather *feather, size_t ownId, int16_t exitcode);

/**
  * Run all threads and wait for their termination.
  * This already calls ft_destroy internally, a further call after this function has returned,
  * is neither necessary nor correct.
  *
  * If exitcodes is non-NULL, the exit codes are written into this array which must have the same length
  * as 'count' specified the thread count on initialization via ft_init.
  */
void ft_run(feather *f, int16_t *exitcodes);

/**
  * Destroys all data structures and stacks.
  * You must only call this function if all user-level threads have exited.
  */
void ft_destroy(feather *f);

/**
  * Print debug information to stdout.
  */
void ft_debug(feather *f);

/**
  * Give up one's CPU time and dispatch another thread.
  */
extern void ft_yield(struct feather* feather);
/**
  * The same as ft_yield except that before switching threads debug information is printed to stdout.
  */
void ft_yieldd(struct feather *f, size_t ownId);

#endif // FEATHER_H