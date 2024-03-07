# ASM_Cours
Cours de Langage d'Assembleur.

## Pre-Requis

1. [installation des packages]
sudo apt install nasm
sudo apt install binutils

ou pour telecharger tout les essentiels
sudo apt install build-essential\

Mettre a jour
sudo apt update

2. [Comande d'Execution]
    Pour Architecture 64 bits
nasm -f elf64 a64.asm -o a64.o
ld a64.o -o a64

    Pour architecture 32 bits
nasm -f elf32 a32.asm -o a32.o
ld a64.o -o a64
Si vous etes sur une architecture 64 mais que vous voulais quand meme le tester
ld -m elf_i386 a32.o -o a32

# Structure d'un programme
etiquette/label: pour etre utiliser par notre lien
Instruction, valeur de destination, valeur source
ou
Instruction, valeur operande

<etiquette>:
    <instruction> <operande(s)>
    <instruction> <operande(s)>
    <instruction> <operande(s)>
    <instruction> <operande(s)>

# Macros

%define hello 'Hello World !'
%assign hello 'Hello World !'
%include 'other.asm'

# Directive 

message_length equ $-message
example times 25 resb 1 (reserve 1 octet 25 fois)

# Processor data sizes
Byte b		= 1-byte,	8 bits		0:255		-128:+127
Word w		= 2-bytes,	16 bits		0:65,535	-32,768:+32,767
Doubleword d	= 4-bytes,	32 bits		0:4,294,967,295	−2,147,483,648:+2,147,483,647
Quadword q	= 8-bytes,	64 bits		0: 2^64-1	-(2^63):2^63-1
Paragraph p	= 16-bytes,	128 bits 	0:2^128-1	-(2^127):2^127-1
Kilobyte Kb	= 1024 bytes
Megabyte Mb	= 1,048,576 bytes

# Declaration map
char		Byte		8-bits		1 byte
short		Word		16-bits		2 bytes		
int		Double-word	32-bits		4 bytes
unsigned int	Double-word	32-bits		4 bytes
long		Quadword	64-bits		8 bytes
long long	Quadword	64-bits		8 bytes
char *		Quadword	64-bits		8 bytes
int *		Quadword	64-bits		8 bytes
float		Double-word	32-bits		4 bytes
double		Quadword	64-bits		8 bytes

# Registre
Architecture		64-bits	| 32-bits | 16-bits | 8-bits hight | 8-bits low

Accumulateur		RAX	 EAX 	   AX	     AH		     AL
Base			RBX	 EBX	   BX	     BH		     BL
Compteur		RCX	 ECX	   CX	     CH		     CL
Donnees			RDX	 EDX	   DX	     DH		     DL
Indice source		RSI	 ESI	   SI			     SIL
Indice Destination	RDI	 EDI	   DI			     DIL
Pointeur pile		RSP	 ES	   SP			     SPL
Pointeur base		RBP	 EBP	   BP			     BPL
registre general	r8	 r8d	   r8w			     r8b
registre general	r9	 r9d	   r9w			     r9b
registre general	...	 ...	   ...			     ...
registre general	r15	 r15d	   r15w			     r15b
Instruct Pointer Reg	RIP

63          31      15   7    0
-------------------------------
|		    |	AX    |
|RAX        |EAX    | AH | AL |
-------------------------------

1. Registre D'Etat (rflags)
Le registre rflags contients des informations concernant le resultat d'execution d'une instruction.
Le registre est mis a jour par le CPU apres chaque instruction, et n'est pas directement
accessible  par le programme, Le registre stock l'information du status de l'instruction
qui a ete executee.
Certains bits du registre appeles drapeaux ont une signification particuliere.

Bits 	63..32 	31	30	29	28	27	26 	25 	24 	23 	22 	
Drapeaux - 	-   	- 	- 	- 	- 	- 	- 	- 	- 	- 

Bits	 21 	20 	19 	18 	17 	16 	15 	14 	13..12 	11 	10 	9
Drapeaux ID 	VIP 	VIF 	AC 	VM 	RF 	0 	NT 	IOPL 	OF 	DF 	IF

Bits	 8 	7 	6 	5 	4 	3 	2 	1 	0
Drapeaux TF 	SF 	ZF 	0 	AF 	0 	PF 	1 	CF

OF DF IF TF SF ZF AF PF CF

Nom	Symbol	Bit	Utilisation
Carry	CF	0	Arme si une op arithm entiere non signee génère une retenue (debordement)
 -   			sur le bit le plus significatif. Aussi utilisé pour l'arithm en precision
 - 			multiple.

Parity	PF	2	Armé si l'octet de poids faible du rés généré après une op arithm contient
 -			un nombre pair de bits à 1.

Adjust	AF	4	Armé si le résultat d'une op arithm génère un résultat provoquant une retenue
 -			sur le troisième bit. Ce drapeau n'est utile que dans l'utilisation du codage BCD

Zero	ZF	6	Armé si le résultat d'une opération arithmétique vaut zéro.

Sign	SF	7	Armé si le résultat d'une opération arithmétique possède un bit de poids fort
 - 			 1, indiquant ainsi un nombre signé négatif. (desarme si res positif).

Direction DF	10	Specifie la direction de l'operation (inc ou dec) de certaines lignes d'op.

Overflow OF	11	Armé si le résultat constitue un nombre positif ou négatif ne pouvant tenir dans
 - 			l'opérande de destination. (pour les ops signee sur entiers).

Registre n16:
r15 q(quadruple word) = 64 bits -> double
r15 d(double word) = 32 bits -> float
r15 w(word) = 16 bits
r15 b(byte) = 8 bits

t -> precision etendu
o
y -> YMM
z -> ZMM

2. Registre de segment
Code Segment CS
Data Segment DS
Stack Segment SS
Extra Segment ES
(...)

Registre precis
511         255       127     0
-------------------------------
|ZMM        |YMM      |XMM    |
-------------------------------

convension d'appel sous intel x86_64:
6 premiers
entier		: rdi, rsi, rdx, rcx, r8, r9, pile (droite -> gauche)
retour fonction	: rax(accumulateur) (64 bits) ou rax + rdx (128 bits)

flotant		: XMM0 à XMM7 (Y pour 256 bits et Z pour 512 bits)
retour fonction	: XMM0 et XMM1 (Y pour 256 bits et Z pour 512 bits)

Architecture ARM pour telephone
MIPS (playstation portable)

Docs
Instruction Set Architecture ISA
Architecture Overview
Reference manual

# Basic Synthax
An assembly program can be divide into 3 sections

1. data section
Utiliser pour declarer des donnees ou contantes initialiser.
Les donnees ne change pas lors du lancement du programme.
type de donnees -> (const valeurs, nom_de_fichier, taile_buffer)
	
	section.data

2. bss section
Utiliser pour declarer des variables.
	
	section.bss

3. text section
Utiliser pour ecrire la partie du code. Cette section doit commencer avec la declaration: 
	global _start
Qui dit ou programme que l'execution commence.
	
	section.text
		global _start
	_start:

4. Syntax du langage assembleur

[label] mnemonic [operands] [;comment]

# Directives de donnees
d{X} <valeur>	->	dans le segment .data
			donnees initialisees
res{X} <valeur>	->	dans le segment .bss
			donnees non initialisees
X depend de la taille des donnes:
	b: 1 octet (byte)
	w: 1 mot (word)
	d: 2 mots (double word)
	q: 4 mots (quadruple word)
	t: 10 octets

# Instructions de deplacement de donnees

mov	dest, src
mov	taille dest, src
movzx	dest, src	extension avec des 0 dans dest
movsx	dest, src	extension avec le bit de signe dans dest

lea	dest, [op]	dest <- adresse de op (load effective address)
push	op		decremente rsp et empile op
pop	op		depile dans op et incremente rsp

# La pile

Last-in First-out
push ajoute une donnees sur la pile de taille qword au sommet de la pile
pop retire la donnee de taille qword au sommet de la pile
Le registre rsp contient l'adresse de la donnee qui se trouve au sommet de la pile
push decremente rsp de 8
pop incremente de 8

#  Prefix et Suffixe des bases numeriques

d ou t : decimal ---> 5, 05, 0150d, 0d150
q ou o : octal ---> 755q, 0q755
b ou y : binaire ---> 0b11011101, 0b1101_1101, 1101_1100b
h ou x : hexa ---> 0xA5, 0A5h

# Instructions arithmetiques

add	op1, op2			op1 <- op1 + op2
adc(avec une retenue)
sub	op1, op2			op1 <- op1 + op2
sbb(avec une retenue)
of(si retenue signee)
cf(si retenue non signee)
neg	reg				reg <- -reg
inc	reg				reg <- reg + 1
dec	reg				reg <- reg - 1
imul	op (signe ou mul non signe)	rdx:rax <- rax * op
imul	dest, op			dest <- dest * op
imul	dest, op, immediat		dest <- op * immediat
idiv	op (div non signe)		rax <- rdx:rax / op, rdx <- rdx:rax mod op 2 (mod op au carre)

and	op1, op2			op1 <- op1 & op2
or	op1, op2			op1 <- op1 | op2
xor	op1, op2			op1 <- op1 ˆ op2
not	reg				reg <- ˜reg
shl	reg, immediat			reg <- reg << immediat
shr	reg, immediat			reg <- reg >> immediat
sal	reg, immediat			reg <- reg << immediat
sar	reg, immediat			reg <- reg >> immediat signe
rol	reg, immediat			reg <- reg decalageCirculaireGaucheDe imm
ror	reg, immediat			reg <- reg decalageCirculaireDroiteDe imm
rcl	reg, immediat			reg : CF <- reg : CF decalageCircGauchede imm
rcr	reg, immediat			reg : CF <- reg : CF decalageCircDroitede imm

cmp	op1, op2			calcul de op1 − op2 (comparaison) et de ZF,CF et OF
jmp	op				branchement (saut) inconditionnel a l’adresse op
jz	op				branchement a l’adresse op si ZF=1
jnz	op				branchement a l’adresse op si ZF=0
jo	op				branchement a l’adresse op si OF=1
jno	op				branchement a l’adresse op si OF=0
js	op				branchement a l’adresse op si SF=1
jns	op				branchement a l’adresse op si SF=0
jc	op				branchement a l’adresse op si CF=1
jnc	op				branchement a l’adresse op si CF=0
jp	op				branchement a l’adresse op si PF=1
jnp	op				branchement a l’adresse op si PF=0

Signees (int): 
je	op				branchement a l’adresse op si op1 = op2
jne	op				branchement a l’adresse op si op1 != op2
jl	op (jnge)			branchement a l’adresse op si op1 < op2
jle	op (jng)			branchement a l’adresse op si op1 ≤ op2
jg	op (jnle)			branchement a l’adresse op si op1 > op2
jge	op (jnl)			branchement a l’adresse op si op1 ≥ op2
Non signees (uint):
je	op				branchement a l’adresse op si op1 = op2
jne	op				branchement a l’adresse op si op1 != op2
jb	op (jnae)			branchement a l’adresse op si op1 < op2
jbe	op (jna)			branchement a l’adresse op si op1 ≤ op2
ja	op (jnbe)			branchement a l’adresse op si op1 > op2
jae	op (jnb)			branchement a l’adresse op si op1 ≥ op2

