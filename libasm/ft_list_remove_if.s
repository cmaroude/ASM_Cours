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
    mov r12, rdi        ; r12 = begin_list (t_list **)

.loop:
    mov rbx, [r12]      ; rbx = *begin_list
    test rbx, rbx
    je .end
    ; cmp((*begin_list)->data, data_ref)
    mov rdi, [rbx]      ; arg1 = node->data
    mov rsi, rsi        ; arg2 = data_ref
    call rdx            ; cmp()
    test eax, eax
    jne .no_remove
    mov r13, [r12]      ; tmp = *begin_list
    mov r14, [r13 + 8]  ; tmp->next
    mov [r12], r14      ; *begin_list = tmp->next
    ; free_fct(tmp->data)
    mov rdi, [r13]
    call rcx
    ; free(tmp)
    mov rdi, r13
    call free
    jmp .loop

.no_remove:
    ; begin_list = &(*begin_list)->next
    mov r12, rbx
    add r12, 8          ; &node->next
    jmp .loop

.end:
    pop r14
    pop r13
    pop r12
    pop rbx
    ret
