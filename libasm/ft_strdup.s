bits 64

extern malloc
extern ft_strlen
extern ft_strcpy
extern __errno_location

section .text
	global ft_strdup

ft_strdup:
	mov r8, rdi
	mov rdi, r8
	call ft_strlen
	inc rax
	mov rdi, rax
	call malloc wrt ..plt
	test rax, rax
	jz .malloc_failed
	
	mov rdi, rax
	mov rsi, r8
	call ft_strcpy
	ret

.malloc_failed:
	call __errno_location wrt ..plt
	mov dword [rax], 12
	mov rax, 0
	ret
