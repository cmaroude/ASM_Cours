bits 64

section .text
	global ft_strcmp

ft_strcmp:
    xor rcx, rcx ; init rcx 0

.loop:
    movzx eax, byte [rdi +rcx] ; passage de 8 a 32 bit
    movzx ebx, byte [rsi + rcx]
    cmp eax, ebx
    jne .diff
    test al, al
    je .equal
    inc rcx
    jmp .loop

.diff:
    sub eax, ebx
    ret

.equal:
    xor eax, eax
    ret
