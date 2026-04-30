bits 64

section .text
	global ft_write
	extern __errno_location

ft_write:
	mov rax, 1
	syscall
	test rax, rax
	jns .done
	
	neg rax ; pour avoir une valeur + si echec syscall
	mov r8, rax
	call __errno_location wrt ..plt
	mov [rax], r8
	mov rax, -1

.done:
	ret
