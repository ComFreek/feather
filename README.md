# Feather

An x86 (IA-32) user-level threads implementation. Cooperative. FCFS strategy. Thread-safe.

The user-level threads act cooperatively, i.e. every thread must voluntarily give up its runtime by either
calling ft_yield, ft_exit or returning from its entry point (equivalent to calling ft_exit).
The scheduler, as implemented in ft_yield, will select the next (as in the one with the incremented index)
thread which has not yet exited.
If all threads have exited, ft_run writes their exit codes into the given parameter (if not NULL) and returns.

This implementation only shares state by means of the feather* pointers passed across various functions and
the usage of malloc/free. Assuming malloc/free are thread-safe, you can safely run user-level threads from
_different_ initialized feather* structures from multiple real threads. Note that user-level threads across real
threads can also share state and communicate among themselves.

## Compatibility Notes:

 1) Currently the only supported compiler is GCC:
    It would most likely work on other compilers with minor modifications,
    e.g. __attribute__ ((...)) directives would need to be ported.
 2) Only works on x86 (IA-32, 32-bit)
 3) The user-level threads must not make use of SSE instructions or SSE registers as these are
    not taken into account when context-switching.

## Trivia
Its name originates from the German word 'Feder' which is sometimes used to refer to user-level-threads
("federgewichtige Threads").
