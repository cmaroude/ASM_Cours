; Programme en architecture 64 bits
bits 64

; Sectoin d'initialisation de donnees
section .data
	message db 'Hello World !', 10 ; code ASCII \n

; Segment de code/texte
section .text
	global _start ; le liant ld attend cette ligne
	_start:
		mov rax, 1 ; Appel systeme write 
		mov rdi, 1 ; Descripteur sur lequel lire =  stdout
		mov rsi, message ; Argument
		mov rdx, 13+1 ; Taille du message
		syscall

		; exit
		mov rax, 60 ; Appel systeme exit
		mov rdi, 0 ; ou xor rdi, rdi
		syscall ; Appel au Kernel
