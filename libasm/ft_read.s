bits 64

extern __errno_location

section .text
	global ft_read

ft_read:
	mov rax, 0
	syscall
	test rax, rax
	jns .done
	
	neg rax ; si syscall echou pour avoir une valeur +
	mov r8, rax
	call __errno_location wrt ..plt
	mov [rax], r8d
	mov rax, -1
	ret

.done:
	ret

