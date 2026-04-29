bits 32
section .data	
	message db 'Hello World !', 10
	
section .text 
	global _start
	_start:
		; entier : ebx, ecx, edx, esi, edi, ebd
		; pile droite -> gauche
		; retour dans eax(accumulateur) (32 bits)
	 	mov eax, 4
		mov ebx, 1
		mov ecx, message
		mov edx, 13+1
		int 0x80
		
		mov eax, 1
		mov ebx, 0 ; xor ebx, ebx
		int 0x80
