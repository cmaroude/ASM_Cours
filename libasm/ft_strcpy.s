bits 64

section .text
	global ft_strcpy

ft_strcpy:
	mov rax, rdi ;sauvegarde dst
	xor rcx, rcx ;init index 0

.loop:
	mov dl, [rsi + rcx] ; lecture dl = src[i]
	mov [rdi + rcx], dl ;dst[i] = src[i]
	test dl, dl ; si == 0
	jz .done
	inc rcx
	jmp .loop
	
.done:
	ret
