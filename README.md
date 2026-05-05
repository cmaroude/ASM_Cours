# 🧠 Cours d’Assembleur (ASM) — x86 / x86_64

Cours **complet, détaillé et exhaustif** de langage assembleur pour apprentissage, reverse engineering, CTF et exploitation.

---

# 📦 1. Installation des outils

## 🛠️ Packages essentiels

```bash
sudo apt update
sudo apt install nasm binutils build-essential gdb
```

### 🔍 Outils importants en plus

* objdump (analyse binaire)
* strace (syscalls)
* ltrace (libc calls)
* gdb-peda / pwndbg (debug avancé)
* ghidra (reverse engineering)
* radare2

````

---

# ⚙️ 2. Compilation et exécution

## 🧬 64 bits
```bash
nasm -g dwarf2 -f elf64 file.asm -o file.o
ld file.o -o file
./file
````

## 🧬 32 bits

```bash
nasm -f elf32 file.asm -o file.o
ld -m elf_i386 file.o -o file
./file
```

---

# 🧱 3. Structure d’un programme ASM

Un programme est divisé en 3 sections :

```asm
section .data   ; données initialisées
section .bss    ; variables non initialisées
section .text   ; code
    global _start
_start:
```

---

# ✍️ 4. Syntaxe générale

```asm
label:
    instruction destination, source ; commentaire
```

### Exemple

```asm
mov rax, rbx
```

---

# 🧠 5. REGISTRES (x86_64 COMPLET)

📞 Appel de fonction et registres (x86_64 - System V ABI)

Quand une fonction est appelée en assembleur (Linux x86_64), les arguments ne sont pas sur la stack au début (contrairement au 32 bits).

👉 Ils sont passés dans des registres spécifiques.

## 🔹 Convention d’appel (System V ABI)

📥 Passage des arguments

| Argument C | Registre |
| ---------- | -------- |
| 1er        | `rdi`    |
| 2e         | `rsi`    |
| 3e         | `rdx`    |
| 4e         | `rcx`    |
| 5e         | `r8`     |
| 6e         | `r9`     |

👉 À partir du 7e argument, ça passe sur la stack
## 🔹 Registres principaux

| Registre | Nom                 | Rôle réel                |
| -------- | ------------------- | ------------------------ |
| RAX      | accumulator         | retour fonction / calcul |
| RBX      | base                | stockage général         |
| RCX      | counter             | boucles                  |
| RDX      | data                | opérations / syscalls    |
| RSI      | source              | pointeur source          |
| RDI      | destination         | pointeur destination     |
| RSP      | stack pointer       | sommet de la pile        |
| RBP      | base pointer        | repère stack frame       |
| RIP      | instruction pointer | instruction courante     |

## 🔹 Registres étendus

```text
r8  → r15 : registres généraux supplémentaires
```

## 🔹 Découpage des registres

| 64 bits | 32 bits | 16 bits | 8 bits  |
| ------- | ------- | ------- | ------- |
| RAX     | EAX     | AX      | AL / AH |
| RBX     | EBX     | BX      | BL / BH |
| RCX     | ECX     | CX      | CL / CH |
| RDX     | EDX     | DX      | DL / DH |

---

# 📊 6. STACK (PILE)

## 🔁 Principe LIFO

* push → empile (RSP -= 8)
* pop → dépile (RSP += 8)

```asm
push rax
pop rbx
```

## 📍 RSP

* pointe le sommet
* change en permanence

---

# 🧱 7. STACK FRAME (TRÈS IMPORTANT)

```asm
push rbp
mov rbp, rsp
sub rsp, 32
```

### 📌 rôle :

* RBP = repère fixe
* RSP = variable

---

# 📍 8. OFFSETS MÉMOIRE

```text
[rbp + 16] → argument 2
[rbp + 8 ] → argument 1
[rbp + 0 ] → ancien rbp
[rbp - 8 ] → variable locale
[rbp - 16] → variable locale
```

👉 Offset = distance depuis rbp ou rsp

---

# 🧠 9. MÉMOIRE & TYPES

| Type C | ASM   | Taille  |
| ------ | ----- | ------- |
| char   | byte  | 8 bits  |
| short  | word  | 16 bits |
| int    | dword | 32 bits |
| long   | qword | 64 bits |

---

# 📦 10. DÉCLARATION MÉMOIRE

## DATA (initialisé)

```asm
db 1
rw 2
dd 4
dq 8
```

## BSS (non initialisé)

```asm
resb 64
resq 1
```

---

# 🔁 11. MOV / TRANSFERT

```asm
mov dest, src
movzx dest, src   ; zero extension
movsx dest, src   ; sign extension
lea rax, [addr]   ; charge adresse
```

---

# ⚙️ 12. INSTRUCTIONS ARITHMÉTIQUES (COMPLÈTES)

## ➕ Addition / soustraction

```asm
add  op1, op2    ; op1 = op1 + op2
adc  op1, op2    ; op1 = op1 + op2 + CF (carry)

sub  op1, op2    ; op1 = op1 - op2
sbb  op1, op2    ; op1 = op1 - op2 - CF
```

👉 Flags affectés : CF, ZF, SF, OF

---

## ➖ Incrément / décrément

```asm
inc reg   ; reg = reg + 1
dec reg   ; reg = reg - 1
neg reg   ; reg = -reg (complément à 2)
```

---

## ✖️ Multiplication

```asm
imul op              ; RDX:RAX = RAX * op
imul dest, op        ; dest = dest * op
imul dest, op, imm   ; dest = op * imm
```

👉 Note :

* version 1 → résultat étendu sur 128 bits (RDX:RAX)

---

## ➗ Division

```asm
idiv op
```

👉 fonctionnement :

* quotient → RAX
* reste → RDX
* dividende = RDX:RAX

⚠️ IMPORTANT : RDX doit être correctement initialisé (souvent 0)

---

## 🔗 Opérations logiques

```asm
and op1, op2   ; ET binaire
or  op1, op2   ; OU binaire
xor op1, op2   ; OU exclusif
not reg        ; inversion bits
```

---

## 🔁 Décalages (shifts)

```asm
shl reg, imm   ; shift gauche (x2)
shr reg, imm   ; shift droite logique
sal reg, imm   ; identique shl
sar reg, imm   ; shift droite arithmétique (garde signe)
```

👉 sar conserve le signe (très important)

---

## 🔄 Rotations

```asm
rol reg, imm   ; rotation gauche
ror reg, imm   ; rotation droite
rcl reg, imm   ; rotation via CF (carry flag)
rcr reg, imm   ; rotation via CF
```

---

# 🔀 13. COMPARAISONS & SAUTS

## 📊 cmp (base de tous les jumps conditionnels)

```asm
cmp op1, op2   ; calcule op1 - op2 sans stocker résultat
```

👉 met à jour les FLAGS : ZF, CF, SF, OF

---

## 🚦 Sauts inconditionnels

```asm
jmp label
```

👉 saute toujours

---

## 🚦 Sauts basés sur FLAGS (généraux)

```asm
jz   label   ; ZF = 1
jnz  label   ; ZF = 0

jo   label   ; OF = 1
jno  label   ; OF = 0

js   label   ; SF = 1
jns  label   ; SF = 0

jc   label   ; CF = 1
jnc  label   ; CF = 0

jp   label   ; PF = 1
jnp  label   ; PF = 0
```

---

## 📌 Sauts SIGNÉS (int)

```asm
je   label   ; égal
jne  label   ; différent

jl   label   ; <  (signed)
jle  label   ; <= (signed)
jg   label   ; >  (signed)
jge  label   ; >= (signed)
```

👉 basés sur SF et OF

---

## 📌 Sauts NON SIGNÉS (uint)

```asm
je   label   ; égal
jne  label   ; différent

jb   label   ; <  (carry)
jbe  label   ; <=
ja   label   ; >
jae  label   ; >=
```

👉 basés sur CF

--- LOGIQUE

```asm
and, or, xor, not
shl, shr
sar
rol, ror
```

---

# 🔄 14. COMPARAISONS

```asm
cmp a, b
je / jne
jg / jl
ja / jb
jge / jle
```

---

# 🚦 15. FLAGS (RFLAGS)

| Flag | Signification |
| ---- | ------------- |
| ZF   | zero          |
| CF   | carry         |
| SF   | sign          |
| OF   | overflow      |
| PF   | parity        |
| AF   | adjust        |
| DF   | direction     |

👉 mis à jour automatiquement par le CPU

---

# 📞 16. APPELS DE FONCTIONS (ABI Linux x86_64)

## Arguments

```text
rdi, rsi, rdx, rcx, r8, r9
```

## Retour

```text
rax
```

---

# ⚠️ 17. SYSCALLS

```asm
mov rax, 60   ; exit
mov rdi, 0
syscall
```

---

# 🧪 18. MACROS

```asm
%define VALUE 10
%include "file.asm"
%assign X 5
```

---

# 🔢 19. BASES NUMÉRIQUES

```text
décimal : 10
hex : 0xA
binaire : 0b1010
octal : 0q12
```

---

# ⚡ 20. ALIGNEMENT MÉMOIRE (CRITIQUE)

👉 Stack DOIT être alignée 16 bytes avant call

```asm
and rsp, -16
```

---

# 🧠 21. MOVZX vs MOVSX

* movzx → zero extension (unsigned)
* movsx → sign extension (signed)

---

# 🔍 22. DEBUG / REVERSE

Outils :

* GDB
* Ghidra
* Radare2

👉 Analyse :

* stack
* registers
* offsets

---

# 💣 23. CONCEPTS CRITIQUES

* stack descend
* RSP change toujours
* RBP = repère fixe
* registres = variables temporaires
* mémoire non protégée

---

# 🔥 24. CONVENTIONS IMPORTANTES

## SysV ABI

* arguments dans registres
* retour dans RAX

## Stack

* push/pop = 8 bytes

---

# 💥 25. CTF / EXPLOITATION

* buffer overflow = dépassement stack
* RIP = contrôle exécution
* offset = distance jusqu’à RIP

---

# 🧠 26. 🧪 EXEMPLES CONCRETS (C ↔ ASM)

## ➕ Addition simple

### C

```c
int a = 5 + 3;
```

### ASM

```asm
mov eax, 5
add eax, 3
```

👉 Flags :

* CF = 0
* ZF = 0

---

## 🔁 Condition if

### C

```c
if (a == b)
    x = 1;
```

### ASM

```asm
cmp rax, rbx
je equal
```

👉 Explication :

* cmp = rax - rbx
* si ZF = 1 → jump

---

## 🔄 Boucle for

### C

```c
for (i = 0; i < 5; i++) {}
```

### ASM

```asm
mov ecx, 5
loop_start:
    dec ecx
    jnz loop_start
```

👉 ECX = compteur

---

# ⚠️ 27. PIÈGES TRÈS IMPORTANTS (CTF / REVERSE)

## 💣 Signed vs Unsigned

```asm
cmp rax, rbx
jl label   ; SIGNÉ
jb label   ; NON SIGNÉ
```

👉 ERREUR CLASSIQUE :

* utiliser jl au lieu de jb → bug logique

---

## 💣 Overflow invisible

```asm
add al, 1
```

👉 problème :

* AL = 8 bits
* dépassement silencieux

CF peut être activé

---

## 💣 mov vs movzx

```asm
mov al, [rdi]
movzx eax, byte [rdi]
```

👉 danger :

* mov laisse les bits hauts sales
* movzx nettoie

---

## 💣 stack corruption

```asm
sub rsp, 8
```

👉 si mal aligné :

* crash syscall
* segfault movaps

---

## 💣 écrasement RIP

Buffer overflow :

```text
buffer → saved rbp → return address (RIP)
```

👉 objectif CTF : contrôler RIP

---

# 🔬 28. IMPACT FLAGS (TRÈS IMPORTANT)

## ➕ add

* CF : carry
* ZF : zéro
* OF : overflow signé

## ➖ sub / cmp

* CF : borrow
* ZF : égalité
* SF : signe
* OF : overflow signé

---

# 🧠 29. MODÈLE MENTAL CPU

👉 Le CPU fait toujours :

```text
fetch → decode → execute → update flags → next RIP
```

---

# 💣 30. COMMENT LES JUMPS RÉELLEMENT FONCTIONNENT

```asm
cmp rax, rbx
je label
```

👉 équivalent CPU :

```text
tmp = rax - rbx
if (tmp == 0) RIP = label
```

---

# 🔥 31. POURQUOI C’EST CRITIQUE EN EXPLOITATION

* contrôler FLAGS = contrôler flux
* contrôler RIP = exécuter code
* stack = mémoire vulnérable

---

# 🧠 FIN

👉 Objectif : comprendre EXACTEMENT comment le CPU exécute un programme.

