bits 64

extern free

section .text
	global ft_list_remove_if

ft_list_remove_if:
	; RDI = t_list **begin_list
	; RSI = data_ref
	; RDX = cmp
	; RCX = free_fct
	push rbx
	push r12
	push r13
	push r14
	push r15
	mov r12, rdi  ; r12 = begin_list (t_list **)
	mov r13, rsi  ; r13 = data_ref
	mov r14, rdx  ; r14 = cmp
	mov r15, rcx	; r15 = free_fct

.loop:
	mov rbx, [r12]	; rbx = *begin_list
	test rbx, rbx
	je .end
	; cmp((*begin_list)->data, data_ref)
	mov rdi, [rbx]	; arg1 = node->data
	mov rsi, r13	; arg2 = data_ref
	sub rsp, 8
	call r14	; cmp()
	add rsp, 8
	test eax, eax
	jne .no_remove
	mov rdx, [rbx + 8]	; next
	mov [r12], rdx      ; *begin_list = tmp->next
	; free_fct(tmp->data)
	mov rdi, [rbx]
	sub rsp, 8
	call r15
	add rsp, 8
	; free(tmp)
	mov rdi, rbx
	sub rsp, 8
	call free wrt ..plt
	add rsp, 8
	jmp .loop

.no_remove:
	; begin_list = &(*begin_list)->next
	lea r12, [rbx + 8]
	jmp .loop

.end:
	pop r15
	pop r14
	pop r13
	pop r12
	pop rbx
	ret
