	.global handler
	.global send_interrupt
	.global pit_handler
	.extern handle_pit_interrupt

handler:
	iretq

send_interrupt:
	int $0x00
	retq

pit_handler:
	push %rax
	push %rcx
	push %rdx
	push %rsi
	push %rdi
	push %r8
	push %r9
	push %r10
	push %r11
	cld
	call handle_pit_interrupt
	pop %r11
	pop %r10
	pop %r9
	pop %r8
	pop %rdi
	pop %rsi
	pop %rdx
	pop %rcx
	pop %rax
	iretq
