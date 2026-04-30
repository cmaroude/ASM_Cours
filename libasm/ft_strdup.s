bits 64

extern malloc
extern ft_strlen
extern ft_strcpy
extern __errno_location

section .text
	global ft_strdup

ft_strdup:
	push rdi ; sauvegarde src, rdi = 1er arg
	call ft_strlen
	inc rax ;rajoute +1 pour \0
	mov rdi, rax
	call malloc wrt ..plt
	test rax, rax
	jz .malloc_failed
	pop rsi ;recupere src
	mov rdi, rax
	call ft_strcpy
	ret

.malloc_failed:
	call __errno_location wrt ..plt
	mov dword [rax], 12 ; error code 12 ENOMEM out of memory
	xor rax, rax ;return null
	add rsp, 8 ; nettoyer le push equ de pop
	ret
