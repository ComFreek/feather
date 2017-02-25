#ifndef FEATHER_H
#define FEATHER_H

/**
  * An x86 (IA-32) user-level threads implementation. Cooperative. FCFS strategy. Thread-safe.
  *
  * The user-level threads act cooperatively, i.e. every thread must voluntarily give up its runtime by either
  * calling ft_yield, ft_exit or returning from its entry point (equivalent to calling ft_exit).
  * The scheduler, as implemented in ft_yield, will select the next (as in the one with the incremented index)
  * thread which has not yet exited.
  * If all threads have exited, ft_run writes their exit codes into the given parameter (if not NULL) and returns.
  *
  * This implementation only shares state by means of the feather* pointers passed across various functions and
  * the usage of malloc/free. Assuming malloc/free are thread-safe, you can safely run user-level threads from
  * _different_ initialized feather* structures from multiple real threads. Note that user-level threads across real
  * threads can also share state and communicate among themselves.
  *
  * Trivia: Its name originates from the German word 'Feder' which is sometimes used to refer to user-level-threads
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
  * Equivalent to returning from the thread's entry point function except that you can
  * everywhere call this function within the thread (e.g. if a helper function called by the entry point
  * function determines that forceful termination is required).
  *
  * The parameters feather and ownId need to match those which where passed to the thread entry point function.
  */
__attribute__((noreturn)) void ft_exit(feather *feather, size_t ownId, int16_t exitcode);

/**
  * Run all threads and wait for their termination.
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