	.file	"feather_test.c"
	.text
.Ltext0:
	.section	.rodata
	.align 4
	.type	STACK_SIZE, @object
	.size	STACK_SIZE, 4
STACK_SIZE:
	.long	4096
	.text
	.globl	ft_init
	.type	ft_init, @function
ft_init:
.LFB0:
	.file 1 "feather.c"
	.loc 1 24 0
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$24, %esp
	.loc 1 25 0
	subl	$12, %esp
	pushl	$12
	call	malloc
	addl	$16, %esp
	movl	%eax, -16(%ebp)
	.loc 1 26 0
	cmpl	$0, -16(%ebp)
	jne	.L2
	.loc 1 28 0
	movl	$0, %eax
	jmp	.L3
.L2:
	.loc 1 31 0
	movl	-16(%ebp), %eax
	movl	8(%ebp), %edx
	movl	%edx, (%eax)
	.loc 1 33 0
	movl	8(%ebp), %edx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	sall	$2, %eax
	subl	$12, %esp
	pushl	%eax
	call	malloc
	addl	$16, %esp
	movl	%eax, %edx
	movl	-16(%ebp), %eax
	movl	%edx, 8(%eax)
	.loc 1 34 0
	movl	-16(%ebp), %eax
	movl	8(%eax), %eax
	testl	%eax, %eax
	jne	.L4
.LBB2:
	.loc 1 35 0
	call	__errno_location
	movl	(%eax), %eax
	movl	%eax, -20(%ebp)
	.loc 1 36 0
	subl	$12, %esp
	pushl	-16(%ebp)
	call	free
	addl	$16, %esp
	.loc 1 37 0
	call	__errno_location
	movl	%eax, %edx
	movl	-20(%ebp), %eax
	movl	%eax, (%edx)
	.loc 1 38 0
	movl	$0, %eax
	jmp	.L3
.L4:
.LBE2:
.LBB3:
	.loc 1 41 0
	movl	$0, -12(%ebp)
	jmp	.L5
.L7:
.LBB4:
	.loc 1 42 0
	movl	$4096, %eax
	subl	$12, %esp
	pushl	%eax
	call	malloc
	addl	$16, %esp
	movl	%eax, -24(%ebp)
	.loc 1 43 0
	cmpl	$0, -24(%ebp)
	jne	.L6
	.loc 1 45 0
	movl	$0, %eax
	jmp	.L3
.L6:
.LBE4:
	.loc 1 41 0 discriminator 2
	addl	$1, -12(%ebp)
.L5:
	.loc 1 41 0 is_stmt 0 discriminator 1
	movl	-12(%ebp), %eax
	cmpl	8(%ebp), %eax
	jb	.L7
.LBE3:
	.loc 1 50 0 is_stmt 1
	movl	-16(%ebp), %eax
.L3:
	.loc 1 51 0
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE0:
	.size	ft_init, .-ft_init
