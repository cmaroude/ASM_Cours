bits 64

section .text
	global ft_list_sort

ft_list_sort:
	; RDI = t_list **begin_list
	; RSI = cmp
	push rbx
	push r12
	push r13
	push r14
	push r15
	mov r15, rsi        ; sauvegarde cmp
	test rdi, rdi
	je .end
	mov r12, [rdi]
	test r12, r12
	je .end

.outer_loop:
	xor r13, r13        ; swapped = 0
	mov rbx, r12

.inner_loop:
	mov r14, [rbx + 8]
	test r14, r14
	je .check_swapped
	; cmp(cur->data, next->data)
	mov rdi, [rbx]
	mov rsi, [r14]
	call r15
	test eax, eax
	jle .no_swap
	; swap
	mov rdx, [rbx]
	mov rcx, [r14]
	mov [rbx], rcx
	mov [r14], rdx
	mov r13, 1

.no_swap:
	mov rbx, r14
	jmp .inner_loop

.check_swapped:
	test r13, r13
	jne .outer_loop

.end:
	pop r15
	pop r14
	pop r13
	pop r12
	pop rbx
	ret
