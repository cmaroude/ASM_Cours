bits 64

section .text
	global ft_list_size

ft_list_size:
    xor rax, rax ; init compteur 0

.loop:
    test rdi, rdi ; si lst == NULL
    jz .done
    inc rax ; i++
    mov rdi, [rdi + 8] ; lst = lst->next
    jmp .loop

.done:
    ret
