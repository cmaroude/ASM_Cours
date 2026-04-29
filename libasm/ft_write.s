bits 64

extern __errno_location

section .text
	global ft_write

ft_write:
	test rsi, rsi
	jz .done
	mov rax, 1
	syscall
	test rax, rax
	jns .done
	
	neg rax
	mov rdi, rax
	call __errno_location
	mov [rax], rdi
	mov rax, -1

.done:
	ret
