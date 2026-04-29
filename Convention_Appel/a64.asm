; conventioncall system V AMD64

;q(quadruple word) = 64 bits -> double
;d(double word) = 32 bits -> float
;w(word) = 16 bits
;b(byte) = 8 bits
 
;t -> precision etendu
;o ->
;y -> YMM
;z -> ZMM
  
;db = data byte <valeur>

;d{x}
;res{x}

; Prefix et Suffixe des bases numeriques
; d ou t : decimal ---> 5, 05, 0150d, 0d150
; q ou o : octal ---> 755q, 0q755
; b ou y : binaire ---> 0b11011101, 0b1101_1101, 1101_1100b
; h ou x : hexa ---> 0xA5, 0A5h

; Formater des caracteres
; message db 'H', 'e', 'l', 'l', 'o', 010d (code decimal 10 ASCII pour rt lg)

bits 64
section .data	
	message db 'Hello World !', 10
	MESSAGE_LEN: equ $-message

section .text 
	global _start
	_start:
		; entier : rdi (destination index), rsi (source index), rdx (donnee), rcx (compteur), r8, r9
		; retour dans rax(accumulateur) (64 bits) ou rax + rdix (128 bits)
		; flottant : XMM0 a XMM7, ou Y sur 256, ou Z sur 512.
		; ret	: XMM0 et XMM1 (Y pour 256 bits et Z pour 512 bits)
		; pile droite -> gauche
	 	mov rax, 1
		mov rdi, 1
		mov rsi, message
		mov rdx, 13+1 ; ou MESSAGE_LEN
		syscall
		
		mov rax, 60
		mov rdi, 0 ; xor rdi, rdi
		syscall
