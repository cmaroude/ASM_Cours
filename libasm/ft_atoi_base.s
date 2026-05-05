bits 64

section .text
	global ft_atoi_base


ft_atoi_base:
	push rbx
	push r12
	push r13
	push r14
	push r15
	mov r12, rdi ; str
	mov r13, rsi ; base
	xor r14, r14 ; base_len = 0

.len_loop:
	mov al, [r13 + r14]
	test al, al
	je .check_base_len
	inc r14
 	jmp .len_loop

.check_base_len:
 	cmp r14, 2
	jl .error
  xor rbx, rbx ; i = 0

.validate_outer:
	cmp rbx, r14
	jge .parse
	mov al, [r13 + rbx]
	; check '+' '-' ou espace
	cmp al, '+'
	je .error
	cmp al, '-'
	je .error
	cmp al, 32 ; espace
	je .error
	cmp al, 9 ; \t
	je .error
	cmp al, 10 ; \n
	je .error
	cmp al, 11 ; \v
	je .error
	cmp al, 12 ; \f
	je .error
	cmp al, 13 ; \r
	je .error
	; check doublons
	mov r15, rbx
	inc r15

.validate_inner:
	cmp r15, r14
	jge .next_char
	mov dl, [r13 + r15]
	cmp al, dl
	je .error
	inc r15
	jmp .validate_inner

.next_char:
	inc rbx
	jmp .validate_outer

.parse:
 	xor rax, rax ; result = 0
	mov rbx, 1 ; sign = 1

.skip_space:
	mov cl, [r12]
	cmp cl, 32
	je .inc_str
	cmp cl, 9
	je .inc_str
	jmp .sign

.inc_str:
	inc r12
	jmp .skip_space

.sign:
	mov cl, [r12]
	cmp cl, '-'
	jne .check_plus
	neg rbx
	inc r12
	jmp .sign

.check_plus:
	cmp cl, '+'
	jne .convert
	inc r12
	jmp .sign

.convert:
	mov cl, [r12]
	test cl, cl
	je .done
	; trouver index dans base
	xor rdx, rdx ; index = 0

.find_in_base:
	cmp rdx, r14
	jge .not_found ; char pas trouvé → stop
	mov r8b, [r13 + rdx]
	cmp r8b, cl
	je .found
	inc rdx
	jmp .find_in_base

.found:
	; result = result * base_len + index
	imul rax, r14
	add rax, rdx
	inc r12
	jmp .convert

.not_found:
	jmp .done

.done:
	imul rax, rbx
	jmp .end

.error:
	xor rax, rax

.end:
	pop r15
	pop r14
	pop r13
	pop r12
	pop rbx
	ret
