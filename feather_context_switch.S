.text
# If you append elements to the feather_thread structure: Change the lines which are commented with
# 'STUCTURE DEPENDENT'
# If you reorder or insert elements in the feather_thread structure: This required more thorough
# rewrite of the whole assembly code!
.globl ft_yield
.globl ft_return_cleanup

ft_return_cleanup: # feather and ownId are still on stack as provided by ft_init()
	# Note that we could use all registers here as we like, but we keep them mostly
	# similar to their use as above in ft_yield

	# Exit code of feather function in %eax

	movl (%esp), %edx
	movl 8(%edx), %ebp # feather_thread * pointer in ebx now
	movl 4(%esp), %esi # ownId
	movl %esi, %edi

	# STUCTURE DEPENDENT: faster than imul $FEATHER_THREAD_SIZE, %edi
	shll $5, %edi

	# Goal: f->threads[ownId].exited = (exitcode << 16) | 0x1;
	lea 24(%ebp, %edi,), %ebx # now pointer to exited in ebx
	shll $16, %eax
	movw $1, %ax
	movl %eax, (%ebx)

	# Continue to ft_yield
	pushl $0xDEADBEEF # fake return address so ft_yield gets the correct stack layout

ft_yield: # extern void ft_yield(struct feather *feather);
	# Assumption: sizeof(size_t) = 4

	# eax, ecx, edx caller-save
	movl 4(%esp), %edx
	movl 8(%edx), %eax # now feather_thread pointer in eax
	movl 4(%edx), %ecx # load ownId
	# STUCTURE DEPENDENT: faster than imul $FEATHER_THREAD_SIZE, %ecx
	shll $5, %ecx
	lea (%eax, %ecx,), %eax # eax := feather_thread* of current (the yielding one) thread

	# Save context
	movl %esp, (%eax)
	movl %ebx, 4(%eax)
	movl %edi, 8(%eax)
	movl %esi, 12(%eax)
	movl %ebp, 16(%eax)
	movl (%esp), %ebp    # Save return address from stack
	movl %ebp, 20(%eax)  # into feather_thread structure

	# Determine next thread
	# STUCTURE DEPENDENT: faster than reload from memory
	shrl $5, %ecx       # ecx = current thread index to be examined
	movl %ecx, %ebx     # Constant ownId copy
	movl 8(%edx), %ebp  # now feather_thread pointer in ebp
next_thread_loop:
	# Goal: ecx = (ecx + 1) % thread_count
	incl %ecx
	cmp %ecx, (%edx)  # Compare against feather->thread_count
	jnz check_exited
	movl $1, %ecx     # Otherwise wrap around to actual user thread 1 (0 is our main thread)

check_exited:
	movl %ecx, %edi
	# STUCTURE DEPENDENT: faster than imul $FEATHER_THREAD_SIZE, %edi
	shll $5, %edi
	movl 24(%ebp, %edi), %eax
	and $0x0001, %eax
	jz new_thread_determined

	# The thread curThreadId has already exited. Now ask: are we (ownId) this thread? If yes, all threads have already exited.
	# => Dispatch main thread
check_circular:
	cmp %ecx, %ebx
	jnz next_thread_loop        # We still haven't walked over the complete list
	xor %ecx, %ecx              # Else, set ecx = 0 (main thread) ...
	xor %edi, %edi              #  and edi = 0 (see new_thread_determined for why)

new_thread_determined: # Expects: %ecx = new threadId, %edi = %ecx * sizeof(*feather->threads)
	movl %ecx, 4(%edx) # store new threadId back into feather structure

	lea (%ebp, %edi,), %eax # now feather_thread struct pointer of new thread in eax

	# Restore context
	movl   (%eax), %esp
	movl  4(%eax), %ebx
	movl  8(%eax), %edi
	movl 12(%eax), %esi
	movl 16(%eax), %ebp

	jmp *20(%eax)
