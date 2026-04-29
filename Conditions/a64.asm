bits 64

; Constantes
FD_OUT equ 1
SYS_EXIT equ 60
SYS_WRITE equ 1

section .data
	n1 dq 5
	n2 dq 5
	message_equals db "n1 est egal a n2", 10
	MSG_EQU_LEN equ $-message_equals
	message_not_equals db "n1 est different de n2", 10
	MSG_NOT_EQU_LEN equ $-message_not_equals


section .text
	global _start
	_start:
		mov rax, [n1]
		mov rbx, [n2]
		cmp rax, rbx
		je equals
		jne not_equals

	equals:
		mov rax, SYS_WRITE
		mov rdi, FD_OUT
		mov rsi, message_equals
		mov rdx, MSG_EQU_LEN
		syscall
		jmp exit_program


	not_equals:	
		mov rax, SYS_WRITE
		mov rdi, FD_OUT
		mov rsi, message_not_equals
		mov rdx, MSG_NOT_EQU_LEN
		syscall
		jmp exit_program


	exit_program:
		; Quitter le prog
		mov rax, SYS_EXIT
		mov rdi, 0
		syscall
