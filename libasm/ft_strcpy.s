bits 64

section .text
	global ft_strcpy

ft_strcpy:
	mov rax, rdi
	xor rcx, rcx

.loop:
	mov dl, [rsi + rcx]
	mov [rdi + rcx], dl
	test dl, dl
	jz .done
	inc rcx
	jmp .loop
	
.done:
	ret
