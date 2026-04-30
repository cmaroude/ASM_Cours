bits 64

section .text
	global ft_strlen

ft_strlen:
	xor rax, rax ; init compteur 0

.loop:
	cmp byte [rdi + rax], 0 ; check si pas == 0
	je .done
	inc rax
	jmp .loop

.done:
	ret
