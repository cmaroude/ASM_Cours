bits 64

section .text
	global ft_strcmp

ft_strcmp:
    xor rcx, rcx

.loop:
    movzx eax, byte [rdi +rcx]
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
