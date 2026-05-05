bits 64

extern malloc

section .text
	global ft_list_push_front

ft_list_push_front:
	; rdi = &begin_list, rsi = data
	test rdi, rdi
	jz .ret
	push rdi
	push rsi
	sub rsp, 8
	mov rdi, 16
	call malloc wrt ..plt ; malloc(sizeof(t_list)) = 16 bytes
	add rsp, 8
	pop rsi
	pop rdi
	test rax, rax
	jz .ret
	mov [rax], rsi ; rax = nouveau node dans lequel on place new->data = data
	mov rcx, [rdi] ; rcx = *begin_list
	mov [rax + 8], rcx ; new->next = old head
	mov [rdi], rax ; *begin_list = new

.ret:
	ret
