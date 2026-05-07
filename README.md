# Cours Complet d'Assembleur x86-64
## Du niveau zéro à la maîtrise : comprendre le langage machine

---

> **Objectif de ce cours** : Te faire comprendre non seulement *comment* fonctionne l'assembleur x86-64, mais surtout *pourquoi* chaque mécanisme existe, comment les pièces s'assemblent, et comment écrire des programmes complexes — allant du bas niveau pur jusqu'aux techniques avancées utilisées en sécurité informatique (virus, antivirus, exploits, reverse engineering).

---

## Table des matières

1. [Avant de commencer : comprendre ce qu'est la machine](#1-avant-de-commencer)
2. [Les registres : les cases mémoire du processeur](#2-les-registres)
3. [Le découpage des registres (visuel)](#3-le-découpage-des-registres)
4. [Les modes d'adressage](#4-les-modes-dadressage)
5. [La mémoire : types, segments, offsets](#5-la-mémoire)
6. [Les déclarations de données](#6-les-déclarations-de-données)
7. [Le transfert de données : MOV et ses variantes](#7-le-transfert-de-données)
8. [Les flags : le registre RFLAGS](#8-les-flags)
9. [Les instructions arithmétiques et logiques](#9-les-instructions-arithmétiques-et-logiques)
10. [Le décalage et la rotation de bits](#10-décalage-et-rotation)
11. [Les comparaisons et les sauts](#11-comparaisons-et-sauts)
12. [La pile (stack) et les frames](#12-la-pile-stack-et-les-frames)
13. [La convention d'appel (calling convention)](#13-la-convention-dappel)
14. [Les appels système (syscall)](#14-les-appels-système-syscall)
15. [Les arguments de ligne de commande](#15-les-arguments-de-ligne-de-commande)
16. [Écrire et assembler un programme complet](#16-écrire-et-assembler-un-programme-complet)
17. [Débogage et reverse engineering](#17-débogage-et-reverse-engineering)
18. [Stack Buffer Overflow](#18-stack-buffer-overflow)
19. [Vers les programmes complexes](#19-vers-les-programmes-complexes)

---

## 1. Avant de commencer

### Qu'est-ce que l'assembleur ?

Quand tu écris du code en Python, en C, ou dans n'importe quel langage dit "de haut niveau", ce code est traduit (compilé ou interprété) en quelque chose que le processeur peut vraiment exécuter. Ce "quelque chose", c'est du **code machine** : une suite de nombres binaires représentant des instructions très précises que le processeur comprend directement.

L'**assembleur** (ou ASM) est la représentation textuelle, lisible par un humain, de ce code machine. Chaque instruction en ASM correspond à **exactement une instruction** que le processeur va exécuter. Il n'y a pas d'abstraction supplémentaire. Quand tu écris `mov rax, 5`, le processeur reçoit une séquence de bits qui lui dit "mets la valeur 5 dans le registre RAX". C'est tout.

C'est pourquoi l'assembleur est à la fois le langage le plus puissant et le plus bas niveau : tu parles directement à la machine, sans intermédiaire.

### Pourquoi apprendre l'ASM ?

On n'écrit presque plus de programmes entiers en ASM de nos jours pour des raisons de productivité. Mais comprendre l'ASM est **indispensable** pour :

- Faire du **reverse engineering** : comprendre ce que fait un programme dont tu n'as pas le code source.
- Exploiter ou corriger des **failles de sécurité** (buffer overflows, ROP chains, shellcode).
- Écrire du code à **performance maximale** dans des sections critiques.
- Comprendre comment fonctionne un **compilateur**, un **débogueur**, un **système d'exploitation**.
- Écrire des **virus**, des **antivirus**, des **rootkits**, des **packers**.

### L'architecture x86-64

L'architecture que tu utilises si tu as un PC moderne (Intel ou AMD) s'appelle **x86-64** (aussi appelée AMD64 ou x64). C'est une extension 64 bits de l'ancienne architecture x86 32 bits.

"64 bits" signifie que les **registres** du processeur font 64 bits de large, que les adresses mémoire font 64 bits, et que le processeur peut manipuler des entiers de 64 bits en une seule instruction.

### Les syntaxes : Intel vs AT&T

Il existe deux syntaxes pour écrire de l'ASM x86-64 :

**Syntaxe Intel** (utilisée par NASM, MASM, et dans la plupart des documentations pédagogiques) :
```nasm
mov rax, 5        ; destination, source — "mets 5 dans RAX"
add rax, rbx      ; destination, source — "ajoute RBX à RAX"
```

**Syntaxe AT&T** (utilisée par GAS, GDB par défaut, et le compilateur GCC) :
```gas
movq $5, %rax     ; source, destination — l'inverse !
addq %rbx, %rax
```

Dans ce cours, nous utiliserons la **syntaxe Intel** avec **NASM** (Netwide Assembler), car elle est plus lisible et intuitive pour débuter. Mais tu rencontreras la syntaxe AT&T dans GDB et dans les sorties de `objdump`, donc il faut la reconnaître.

La règle de conversion AT&T → Intel : inverse la destination et la source, enlève les `%` et `$`, et ignore les suffixes de taille (`b`, `w`, `l`, `q`).

### Le cycle fetch-decode-execute

Pour comprendre comment le processeur exécute du code, il faut comprendre ce cycle fondamental :

1. **Fetch** : le processeur lit la prochaine instruction en mémoire, à l'adresse pointée par le registre `RIP` (Instruction Pointer).
2. **Decode** : il interprète les octets lus pour comprendre quelle instruction c'est et quels sont ses opérandes.
3. **Execute** : il exécute l'instruction.
4. **RIP avance** au nombre d'octets de l'instruction exécutée, et on recommence.

`RIP` est le registre le plus important conceptuellement : c'est le "curseur" qui dit où le processeur en est dans son exécution. Modifier `RIP` (avec un saut ou un appel de fonction), c'est changer le flot d'exécution.

---

## 2. Les registres

### Qu'est-ce qu'un registre ?

Un registre est une **case mémoire ultra-rapide** intégrée directement dans le processeur. Contrairement à la RAM (qui est externe au CPU et met plusieurs dizaines ou centaines de cycles pour répondre), un registre est accessible en **1 cycle**. C'est pourquoi toutes les opérations du processeur se font *dans* les registres — on charge une valeur depuis la RAM dans un registre, on la manipule dans le registre, et on la réécrit en RAM si nécessaire.

En x86-64, il existe plusieurs catégories de registres.

### Les registres généraux (General Purpose Registers — GPR)

Ce sont les 16 registres principaux que tu utiliseras pour la quasi-totalité de tes opérations. Ils font chacun 64 bits.

| Registre 64 bits | Nom historique | Rôle principal (convention) |
|-----------------|----------------|----------------------------|
| `RAX`           | Accumulator    | Valeur de retour des fonctions, résultat des opérations |
| `RBX`           | Base           | Pointeur de base (callee-saved) |
| `RCX`           | Counter        | Compteur de boucles, 4ème argument |
| `RDX`           | Data           | Extension de RAX pour mul/div, 3ème argument |
| `RSI`           | Source Index   | 2ème argument, pointeur source |
| `RDI`           | Dest Index     | 1er argument, pointeur destination |
| `RSP`           | Stack Pointer  | Sommet de la pile — **ne jamais utiliser librement** |
| `RBP`           | Base Pointer   | Base de la frame courante |
| `R8`            | —              | 5ème argument |
| `R9`            | —              | 6ème argument |
| `R10`           | —              | Scratch (caller-saved) |
| `R11`           | —              | Scratch (caller-saved) |
| `R12`           | —              | Callee-saved |
| `R13`           | —              | Callee-saved |
| `R14`           | —              | Callee-saved |
| `R15`           | —              | Callee-saved |

Le terme "rôle principal" ici renvoie à la **convention d'appel** (chapitre 13). Le processeur n'impose pas ces rôles matériellement — c'est un accord entre programmeurs et compilateurs pour que les fonctions puissent s'appeler sans se marcher dessus.

### Le registre des flags : RFLAGS

`RFLAGS` est un registre 64 bits dont chaque **bit** a une signification particulière. Il est mis à jour automatiquement après la plupart des opérations arithmétiques et logiques. On en parlera en détail au chapitre 8.

### Le registre instruction pointer : RIP

`RIP` contient l'adresse de la **prochaine instruction à exécuter**. On ne peut pas y écrire directement avec un `mov` — on le modifie via des instructions de saut (`jmp`, `call`, `ret`).

### Les registres de segments (héritage)

Ces registres (`CS`, `DS`, `SS`, `ES`, `FS`, `GS`) sont des vestiges de l'architecture 16 bits. En mode 64 bits, la plupart sont obsolètes. Seuls `FS` et `GS` restent utilisés — notamment par le noyau Linux pour pointer vers des structures internes (TLS — Thread Local Storage).

### Les registres SIMD : XMM, YMM, ZMM

Ces registres servent à faire des opérations **vectorielles** : traiter plusieurs valeurs en parallèle. `XMM0` à `XMM15` font 128 bits. `YMM0` à `YMM15` font 256 bits (extension AVX). `ZMM0` à `ZMM31` font 512 bits (extension AVX-512). Ils sont utilisés pour le calcul flottant et le calcul massivement parallèle. On ne les couvrira pas en détail dans ce cours d'introduction, mais sache qu'ils existent et que `XMM0` est aussi utilisé pour le retour de valeurs flottantes.

---

## 3. Le découpage des registres

C'est l'une des parties les plus importantes à comprendre car elle crée beaucoup de confusion chez les débutants. Les registres x86-64 ne sont pas des entités séparées : ils se **chevauchent**. Un grand registre 64 bits contient en lui-même ses versions 32 bits, 16 bits, et 8 bits.

### Visualisation complète

```
Registre RAX (64 bits) :
┌─────────────────────────────────────────────────────────────────┐
│                              RAX                                │
│  63                          32│31              16│15    8│7   0│
│                                │                 │       │     │
│◄──────────────── 64 bits ─────────────────────────────────────►│
│                                │◄─── EAX (32) ──────────────►  │
│                                                 │◄── AX (16)►  │
│                                                         │AH│AL │
│                                                         └──┘└──┘
│                                                         8b  8b  │
└─────────────────────────────────────────────────────────────────┘

Accès :
  RAX  → 64 bits  (bits 63..0)
  EAX  → 32 bits  (bits 31..0)
  AX   → 16 bits  (bits 15..0)
  AH   →  8 bits  (bits 15..8)   "High byte"
  AL   →  8 bits  (bits  7..0)   "Low byte"
```

```
Registre RBX :
  RBX → 64 bits
  EBX → 32 bits
  BX  → 16 bits
  BH  →  8 bits (high)
  BL  →  8 bits (low)

Registre RCX :
  RCX → ECX → CX → CH / CL

Registre RDX :
  RDX → EDX → DX → DH / DL

Registre RSI :
  RSI → ESI → SI → (pas de SH) → SIL (8 bits low)

Registre RDI :
  RDI → EDI → DI → DIL (8 bits low)

Registre RSP :
  RSP → ESP → SP → SPL (8 bits low)

Registre RBP :
  RBP → EBP → BP → BPL (8 bits low)
```

Pour les registres R8 à R15, la notation change :

```
Registre R8 :
  R8   → 64 bits
  R8D  → 32 bits  (le D signifie "doubleword", 32 bits)
  R8W  → 16 bits  (le W signifie "word", 16 bits)
  R8B  →  8 bits  (le B signifie "byte", 8 bits)

Même schéma pour R9..R15 : R9D, R9W, R9B, etc.
```

### Le comportement crucial : que se passe-t-il aux autres bits ?

Quand tu écris dans un sous-registre, que se passe-t-il aux bits restants ? La réponse dépend de la taille :

**Écriture dans un registre 32 bits (ex: EAX)** → les 32 bits supérieurs de RAX sont **mis à zéro automatiquement**.

```nasm
mov rax, 0xFFFFFFFFFFFFFFFF   ; RAX = 0xFFFF_FFFF_FFFF_FFFF
mov eax, 0x12345678           ; RAX = 0x0000_0000_1234_5678 (!!!)
                              ; les 32 bits hauts sont effacés
```

C'est un comportement **intentionnel** introduit dans x86-64 pour simplifier la gestion des registres.

**Écriture dans un registre 16 bits (ex: AX)** → les bits restants sont **préservés**.

```nasm
mov rax, 0xFFFFFFFFFFFFFFFF   ; RAX = 0xFFFF_FFFF_FFFF_FFFF
mov ax, 0x1234                ; RAX = 0xFFFF_FFFF_FFFF_1234
                              ; seuls les 16 bits bas changent
```

**Écriture dans un registre 8 bits (ex: AL ou AH)** → les bits restants sont **préservés**.

```nasm
mov rax, 0xFFFFFFFFFFFFFFFF
mov al, 0x42                  ; RAX = 0xFFFF_FFFF_FFFF_FF42
mov ah, 0x00                  ; RAX = 0xFFFF_FFFF_FFFF_0042
```

### Pourquoi c'est important ?

Ce comportement est à l'origine de nombreux bugs subtils. Par exemple, si tu fais un calcul dans EAX en pensant que RAX sera propre, et que tu avais des données dans les bits hauts, tu vas te retrouver avec des résultats corrompus si tu utilises des instructions 16 bits sans t'en rendre compte.

La bonne pratique : **toujours travailler en 64 bits** (`rax`, `rbx`, etc.) sauf raison spécifique. Et si tu dois travailler en 32 bits, sache que ça nettoie automatiquement les bits hauts.

### Tableau récapitulatif de toutes les déclinaisons

```
┌────────┬────────┬──────┬──────┬──────┐
│ 64-bit │ 32-bit │16-bit│ 8-hi │ 8-lo │
├────────┼────────┼──────┼──────┼──────┤
│ RAX    │ EAX    │ AX   │ AH   │ AL   │
│ RBX    │ EBX    │ BX   │ BH   │ BL   │
│ RCX    │ ECX    │ CX   │ CH   │ CL   │
│ RDX    │ EDX    │ DX   │ DH   │ DL   │
│ RSI    │ ESI    │ SI   │  —   │ SIL  │
│ RDI    │ EDI    │ DI   │  —   │ DIL  │
│ RSP    │ ESP    │ SP   │  —   │ SPL  │
│ RBP    │ EBP    │ BP   │  —   │ BPL  │
│ R8     │ R8D    │ R8W  │  —   │ R8B  │
│ R9     │ R9D    │ R9W  │  —   │ R9B  │
│ R10    │ R10D   │ R10W │  —   │ R10B │
│ R11    │ R11D   │ R11W │  —   │ R11B │
│ R12    │ R12D   │ R12W │  —   │ R12B │
│ R13    │ R13D   │ R13W │  —   │ R13B │
│ R14    │ R14D   │ R14W │  —   │ R14B │
│ R15    │ R15D   │ R15W │  —   │ R15B │
└────────┴────────┴──────┴──────┴──────┘
```

---

## 4. Les modes d'adressage

Un mode d'adressage décrit **comment on spécifie l'opérande** d'une instruction. En ASM, quand tu écris `mov rax, quelquechose`, le "quelquechose" peut être de plusieurs natures. Le processeur doit savoir s'il doit utiliser une valeur directe, le contenu d'un registre, ou quelque chose en mémoire.

### Mode immédiat (Immediate)

La valeur est **directement encodée** dans l'instruction. C'est le cas le plus simple.

```nasm
mov rax, 42          ; RAX = 42 (valeur directe)
mov rbx, 0xFF        ; RBX = 255
mov rcx, 0b1010      ; RCX = 10 (binaire)
add rax, 1           ; RAX = RAX + 1
```

La valeur est "immédiate" car elle fait partie de l'instruction elle-même, elle ne vient pas d'un registre ni de la mémoire.

### Mode registre (Register)

L'opérande est le **contenu d'un registre**.

```nasm
mov rax, rbx         ; RAX = contenu de RBX
add rax, rcx         ; RAX = RAX + RCX
```

### Mode mémoire direct (Direct Memory)

On accède à une adresse mémoire **fixe**, connue à la compilation. En NASM, les crochets `[...]` signifient "va lire à cette adresse".

```nasm
mov rax, [0x401000]  ; RAX = valeur à l'adresse 0x401000
mov [var], rax       ; écrit RAX à l'adresse de 'var'
```

Ici, `var` est un label (une adresse) défini dans notre programme. On accède directement à cette case mémoire.

### Mode registre indirect (Register Indirect)

L'adresse est contenue **dans un registre**. Le registre joue le rôle d'un pointeur.

```nasm
mov rax, [rbx]       ; RAX = valeur à l'adresse contenue dans RBX
                     ; RBX est un pointeur, [RBX] le déréférence
```

C'est l'équivalent ASM de l'opérateur `*` en C : si `rbx` contient l'adresse 0x1000, alors `[rbx]` lit 8 octets à l'adresse 0x1000.

### Mode base + déplacement (Base + Displacement)

On accède à l'adresse `registre + constante`. C'est le mode le plus utilisé pour accéder aux champs d'une structure ou aux variables locales sur la pile.

```nasm
mov rax, [rbx + 8]   ; RAX = valeur à l'adresse RBX + 8
mov rax, [rbp - 16]  ; accès à une variable locale (16 octets sous RBP)
```

La constante (appelée "déplacement" ou "offset") peut être positive ou négative.

### Mode base + index (Base + Index * Scale)

Le mode le plus général. La formule est :

```
[base + index * scale + displacement]
```

Où :
- `base` : n'importe quel registre général
- `index` : n'importe quel registre général **sauf RSP**
- `scale` : 1, 2, 4, ou 8 (pour pointer dans des tableaux d'éléments de 1, 2, 4, ou 8 octets)
- `displacement` : constante signée

```nasm
; Accès au 3ème élément d'un tableau d'entiers 64 bits
; base = rbx (adresse du tableau), index = rcx (indice), scale = 8 (taille d'un int64)
mov rax, [rbx + rcx * 8]

; Accès au 2ème champ d'un tableau de structures de 16 octets
mov rax, [rbx + rcx * 16 + 8]
```

C'est extrêmement puissant : en une seule instruction, le processeur peut calculer une adresse complexe et y accéder. C'est pour ça qu'on dit que x86 a des "modes d'adressage riches".

### La taille de l'accès mémoire : les spécificateurs

Quand on accède à de la mémoire, le processeur doit savoir combien d'octets lire ou écrire. Si le registre destination est explicite, il le déduit du registre :

```nasm
mov rax, [rbx]    ; lit 8 octets (64 bits) car RAX est 64 bits
mov eax, [rbx]    ; lit 4 octets (32 bits) car EAX est 32 bits
mov ax,  [rbx]    ; lit 2 octets (16 bits)
mov al,  [rbx]    ; lit 1 octet  (8 bits)
```

Mais parfois, on veut écrire en mémoire sans registre source évident, et on doit être explicite :

```nasm
mov byte  [rbx], 42   ; écrit 1 octet  (8 bits)
mov word  [rbx], 42   ; écrit 2 octets (16 bits)
mov dword [rbx], 42   ; écrit 4 octets (32 bits)
mov qword [rbx], 42   ; écrit 8 octets (64 bits)
```

---

## 5. La mémoire

### L'espace d'adressage

En 64 bits, théoriquement, on peut adresser 2^64 octets soit 16 exaoctets. En pratique, les systèmes actuels n'utilisent que les 48 bits inférieurs des adresses (soit 256 To). Les adresses valides sont donc dans la plage `0x0000_0000_0000_0000` à `0x0000_7FFF_FFFF_FFFF` pour l'espace utilisateur, et `0xFFFF_8000_0000_0000` à `0xFFFF_FFFF_FFFF_FFFF` pour le noyau.

### Organisation de la mémoire d'un processus Linux

Quand un programme est lancé, le noyau lui alloue un espace d'adressage virtuel organisé en **segments** (régions) distincts :

```
Adresses hautes
┌────────────────────────────────┐  0xFFFF_FFFF_FFFF_FFFF
│         Espace noyau           │  (inaccessible depuis l'espace user)
├────────────────────────────────┤  0x0000_7FFF_FFFF_FFFF
│                                │
│     Arguments / Environnement  │  argv, environ (au démarrage)
│                                │
├────────────────────────────────┤
│                                │
│      STACK (pile)              │  ↓ croît vers le bas
│      (variables locales,       │
│       adresses de retour,      │
│       arguments de fonctions)  │
├────────────────────────────────┤
│                                │
│      (espace libre)            │
│                                │
├────────────────────────────────┤
│      MMAP (bibliothèques,      │  libc.so, etc.
│            fichiers mappés)    │
├────────────────────────────────┤
│                                │
│      (espace libre)            │
│                                │
├────────────────────────────────┤
│      HEAP (tas)                │  ↑ croît vers le haut
│      (malloc, new, etc.)       │
├────────────────────────────────┤
│      BSS                       │  Variables globales non initialisées
├────────────────────────────────┤
│      DATA (.data)              │  Variables globales initialisées
├────────────────────────────────┤
│      TEXT (.text)              │  Le code exécutable du programme
└────────────────────────────────┘  0x0000_0000_0040_0000 (souvent)
                                    0x0000_0000_0000_0000 (NULL, non mappé)
Adresses basses
```

### Le segment `.text`

C'est là où vit le **code machine** de ton programme. Il est en lecture seule (on ne peut pas l'écrire pendant l'exécution — enfin, normalement) et exécutable. Quand tu écris une fonction en ASM, elle va dans `.text`.

### Le segment `.data`

Les **variables globales initialisées**. Par exemple, une chaîne de caractères que tu déclares dans ton programme avec une valeur dès le départ.

```nasm
section .data
    message db "Hello, World!", 10, 0   ; chaîne avec newline et null-terminator
    nombre  dq 42                       ; entier 64 bits valant 42
```

### Le segment `.bss`

Les **variables globales non initialisées** (ou initialisées à zéro). Le noyau garantit que cette zone est remplie de zéros au démarrage. En termes de taille de fichier binaire, `.bss` ne prend aucune place — juste une indication "réserve X octets".

```nasm
section .bss
    buffer resb 1024    ; réserve 1024 octets (non initialisés)
    tableau resq 10     ; réserve 10 qwords (80 octets)
```

### La heap (tas)

La heap est la zone de mémoire allouée **dynamiquement** pendant l'exécution via `malloc` (en C) ou directement via l'appel système `brk`/`mmap`. En assembleur pur, on peut accéder à la heap via des syscalls. La heap croît vers le haut (vers les adresses plus hautes).

### La stack (pile)

La pile est abordée en détail au chapitre 12, mais retiens que :
- Elle croît **vers le bas** (vers les adresses basses).
- `RSP` pointe toujours vers le **sommet actuel** de la pile (le dernier élément empilé).
- Elle stocke les variables locales, les adresses de retour, et les arguments de fonctions.

### La mémoire virtuelle vs physique

Chaque processus a son propre espace d'adressage **virtuel**. L'adresse `0x401000` dans le processus A n'est pas le même octet physique que `0x401000` dans le processus B. C'est le MMU (Memory Management Unit) du processeur, aidé par le noyau, qui traduit les adresses virtuelles en adresses physiques via des tables de pages. Cela donne l'isolation entre processus et permet des fonctionnalités comme la mémoire partagée.

### Les offsets de mémoire

Un offset est un **déplacement** par rapport à une adresse de base. Par exemple, dans une structure C :

```c
struct Exemple {
    int  champ_a;    // offset 0
    int  champ_b;    // offset 4
    long champ_c;    // offset 8
};
```

Pour accéder à `champ_b` d'un objet dont l'adresse est dans `rbx` :

```nasm
mov eax, [rbx + 4]   ; 4 = offset de champ_b dans la structure
```

Le compilateur C calcule ces offsets automatiquement. En ASM, tu dois les calculer toi-même (ou les lire dans le débogueur / les définitions de structures).

### L'endianness : petit ou grand boutiste

x86-64 est **little-endian** : les octets d'un entier multi-octets sont stockés du **moins significatif au plus significatif**.

Exemple : la valeur 0x12345678 stockée à l'adresse 0x1000 :

```
Adresse : 0x1000  0x1001  0x1002  0x1003
Valeur  :   0x78    0x56    0x34    0x12
```

Le byte le moins significatif (`0x78`) est en premier (à l'adresse la plus basse). C'est contre-intuitif au début mais c'est ce que tu verras dans tout débogueur sur architecture x86.

---

## 6. Les déclarations de données

### Les types de données en NASM

NASM (et l'ASM en général) ne connaît pas les types au sens C/Python. Tout est une suite d'octets. Mais il fournit des directives pour déclarer des données de tailles différentes.

**Dans `.data` (données initialisées) :**

```nasm
section .data
    ; db = define byte (1 octet)
    monByte     db 0x41              ; 1 octet : valeur 0x41 = 'A'
    caractere   db 'A'              ; idem, notation caractère

    ; dw = define word (2 octets)
    monWord     dw 0x1234           ; 2 octets

    ; dd = define dword (4 octets, "double word")
    monDword    dd 0x12345678       ; 4 octets

    ; dq = define qword (8 octets, "quad word")
    monQword    dq 0x123456789ABCDEF0  ; 8 octets

    ; Chaînes de caractères
    ; 'H','e','l','l','o' puis 10 (newline '\n') puis 0 (null terminator)
    bonjour     db "Bonjour", 10, 0

    ; La longueur d'une chaîne peut être calculée automatiquement
    message     db "Hello, World!", 10
    msg_len     equ $ - message     ; $ = adresse courante, msg_len = longueur
```

**Dans `.bss` (données non initialisées) :**

```nasm
section .bss
    ; resb = reserve bytes (réserver des octets)
    tampon      resb 256    ; 256 octets
    octet       resb 1      ; 1 octet

    ; resw = reserve words
    mots        resw 10     ; 10 words = 20 octets

    ; resd = reserve dwords
    entiers     resd 5      ; 5 dwords = 20 octets

    ; resq = reserve qwords
    grands      resq 4      ; 4 qwords = 32 octets
```

### Les constantes avec `equ`

`equ` définit une constante symbolique, un peu comme `#define` en C. Elle ne prend aucune place en mémoire — c'est juste un remplacement textuel pour l'assembleur.

```nasm
STDIN   equ 0
STDOUT  equ 1
STDERR  equ 2
SYS_WRITE equ 1
SYS_EXIT  equ 60
```

### Les labels

Un label est un **nom** que tu donnes à une adresse. Quand l'assembleur rencontre un label comme `message:`, il associe ce nom à l'adresse courante. Tu peux ensuite utiliser ce nom dans le code pour y faire référence.

```nasm
section .data
    message:               ; "message" pointe ici
        db "Hello", 0

section .text
    global _start
_start:
    lea rdi, [rel message]  ; charge l'adresse de 'message' dans RDI
```

---

## 7. Le transfert de données

### L'instruction MOV

`MOV` est l'instruction la plus fondamentale de l'ASM. Elle copie une valeur d'une source vers une destination. La syntaxe Intel est :

```nasm
mov destination, source
```

**Règles critiques de MOV :**

1. La destination et la source doivent être de la **même taille**.
2. On ne peut pas faire `mov [mem1], [mem2]` — **jamais deux opérandes mémoire** en même temps. On doit passer par un registre.
3. On ne peut pas mettre une constante 64 bits dans un opérande mémoire directement.

```nasm
; Registre ← Immédiat
mov rax, 42
mov al, 0xFF

; Registre ← Registre
mov rbx, rax

; Registre ← Mémoire
mov rax, [rbx]
mov eax, [rbp - 8]

; Mémoire ← Registre
mov [rbx], rax
mov [rbp - 8], ecx

; Mémoire ← Immédiat (taille explicite requise)
mov byte [rbx], 42
mov qword [rbp - 8], 0

; INTERDIT :
; mov [mem1], [mem2]   ← erreur, deux mémoires
; mov al, ax           ← erreur, tailles différentes
```

### MOV avec extension de signe et de zéro

Quand on veut copier une valeur plus petite dans un registre plus grand, on a le choix :

**MOVZX — MOV with Zero eXtension** : remplit les bits hauts avec des **zéros**.

```nasm
; Charge un byte depuis la mémoire dans RAX en remplissant de zéros
movzx rax, byte [rbx]    ; si [rbx] = 0xFF → RAX = 0x00000000000000FF
movzx eax, ax            ; étend AX (16 bits) à EAX (32 bits) avec des zéros
```

**MOVSX — MOV with Sign eXtension** : remplit les bits hauts en **dupliquant le bit de signe** (bit le plus haut de la source). Nécessaire pour les entiers signés.

```nasm
; Si [rbx] = 0xFF (= -1 en signé sur 8 bits)
movsx rax, byte [rbx]    ; RAX = 0xFFFFFFFFFFFFFFFF (= -1 sur 64 bits)
; Si [rbx] = 0x7F (= 127 en signé sur 8 bits)
movsx rax, byte [rbx]    ; RAX = 0x000000000000007F (= 127 sur 64 bits)
```

Pourquoi c'est important ? Parce que -1 sur 8 bits c'est `0xFF`. Si tu fais un `movzx`, tu obtiens `0x00FF` = 255 (faux). Si tu fais un `movsx`, tu obtiens `0xFFFFFFFF...FF` = -1 (correct pour les entiers signés).

**MOVSXD** : version spéciale pour étendre un DWORD (32 bits) en QWORD (64 bits) de façon signée.

```nasm
movsxd rax, eax     ; étend EAX signé vers RAX
movsxd rax, dword [rbx]
```

### XCHG — échange de valeurs

Échange atomiquement le contenu de deux opérandes. Utile pour la synchronisation.

```nasm
xchg rax, rbx    ; RAX et RBX échangent leurs valeurs
```

### LEA — Load Effective Address

`LEA` est une instruction d'une richesse redoutable. Elle **calcule** une adresse effective (en utilisant la syntaxe des modes d'adressage) et place le résultat dans un registre, **sans accéder à la mémoire**.

```nasm
lea rax, [rbx + rcx * 8 + 16]
; RAX = RBX + RCX * 8 + 16
; AUCUN accès mémoire n'a eu lieu !
```

Pourquoi c'est utile ? Parce que cela permet de faire des calculs en une seule instruction, en exploitant le matériel de calcul d'adresse du processeur :

```nasm
; Calculer RAX = RBX + 3 * RCX sans utiliser de mul
lea rax, [rbx + rcx * 2]   ; RAX = RBX + 2*RCX
add rax, rcx               ; RAX = RBX + 3*RCX

; Avancer un pointeur de 16 octets
lea rsi, [rsi + 16]         ; équivalent à add rsi, 16 mais parfois plus rapide

; Charger l'adresse d'un label (attention : nécessite rel pour PIE)
lea rdi, [rel message]     ; RDI = adresse de 'message'
```

`LEA` est tellement flexible que les compilateurs modernes l'utilisent massivement pour l'arithmétique, pas seulement pour calculer des adresses.

### CMOVcc — MOV conditionnel

Les instructions `CMOVcc` ("Conditional MOV") effectuent un `mov` seulement si une condition est vraie (selon les flags). Elles permettent d'éviter des branchements (utile pour éviter les mispredictions de branche).

```nasm
cmp rax, rbx
cmovg rcx, rdx    ; si RAX > RBX (signed), RCX = RDX
cmove rax, rbx    ; si égalité, RAX = RBX
```

---

## 8. Les flags

### Le registre RFLAGS

`RFLAGS` est un registre 64 bits dont la plupart des bits représentent des "drapeaux" (flags) qui reflètent l'état du résultat de la dernière opération. Ces flags sont automatiquement mis à jour par des opérations arithmétiques, logiques, et de comparaison. Ils sont ensuite lus par les instructions de saut conditionnel.

Voici la carte des bits de RFLAGS :

```
Bit 63..22 : réservés / non utilisés en espace utilisateur
Bit 21 : ID   — support CPUID
Bit 20 : VIP  — Virtual Interrupt Pending
Bit 19 : VIF  — Virtual Interrupt Flag
Bit 18 : AC   — Alignment Check
Bit 17 : VM   — Virtual-8086 Mode
Bit 16 : RF   — Resume Flag

Bit 14 : NT   — Nested Task

Bit 13-12 : IOPL — I/O Privilege Level

Bit 11 : OF  — Overflow Flag        ← IMPORTANT
Bit 10 : DF  — Direction Flag       ← IMPORTANT (pour les string ops)
Bit  9 : IF  — Interrupt Flag       ← géré par le noyau
Bit  8 : TF  — Trap Flag            ← débogage pas-à-pas
Bit  7 : SF  — Sign Flag            ← IMPORTANT
Bit  6 : ZF  — Zero Flag            ← IMPORTANT
Bit  4 : AF  — Auxiliary Carry Flag (pour BCD, rarissime)
Bit  2 : PF  — Parity Flag
Bit  0 : CF  — Carry Flag           ← IMPORTANT
```

Concentrons-nous sur les flags réellement importants en pratique quotidienne.

### CF — Carry Flag (drapeau de retenue)

Le **Carry Flag** est mis à 1 quand une opération produit une **retenue** (carry) ou un **emprunt** (borrow) — c'est-à-dire quand le résultat arithmétique déborde de la taille du registre, sans tenir compte du signe.

**Quand CF est mis à 1 :**

- **Addition non signée** : quand le résultat dépasse la valeur maximale non signée.
  ```nasm
  mov rax, 0xFFFFFFFFFFFFFFFF   ; max uint64 = 2^64 - 1
  add rax, 1                    ; résultat = 2^64, ne rentre pas → CF = 1, RAX = 0
  ```
- **Soustraction non signée** : quand le minuende est inférieur au subtrahende (emprunt).
  ```nasm
  mov rax, 5
  sub rax, 10    ; 5 - 10 = -5, impossible en non signé → CF = 1
  ```
- **Rotation/décalage** : le bit sorti à gauche ou à droite se retrouve dans CF.

**Quand CF est mis à 0 :**
- Quand l'opération ne produit pas de débordement non signé.

**Utilisation :** CF est le flag clé pour les **opérations non signées** et pour les **additions multi-précision** (ADC).

### ZF — Zero Flag (drapeau de zéro)

Le **Zero Flag** est mis à 1 quand le résultat d'une opération est **exactement zéro**.

```nasm
mov rax, 5
sub rax, 5     ; 5 - 5 = 0 → ZF = 1

mov rax, 0
add rax, 0     ; 0 + 0 = 0 → ZF = 1

xor rax, rax   ; RAX = RAX XOR RAX = 0 → ZF = 1 (truc classique pour mettre rax à 0)

mov rax, 5
sub rax, 3     ; 5 - 3 = 2 ≠ 0 → ZF = 0
```

**Utilisation :** ZF est utilisé pour tester l'égalité (`cmp a, b` → si a == b, ZF = 1) et pour les boucles (`loop` jusqu'à ce que le compteur soit 0).

### SF — Sign Flag (drapeau de signe)

Le **Sign Flag** est mis à 1 quand le résultat d'une opération est **négatif**, c'est-à-dire quand son bit le plus haut (bit de signe en complément à deux) est 1.

```nasm
mov rax, 0
sub rax, 1    ; 0 - 1 = -1 → SF = 1 (le résultat 0xFFFF...FFFF a le bit 63 à 1)

mov rax, 5
sub rax, 3    ; 5 - 3 = 2 → SF = 0 (résultat positif)

mov rax, 0x8000000000000000  ; valeur dont le bit 63 est 1 → SF = 1
```

**Utilisation :** SF est utilisé pour les comparaisons **signées** en combinaison avec OF.

### OF — Overflow Flag (drapeau de débordement)

Le **Overflow Flag** est mis à 1 quand une opération sur des **entiers signés** produit un résultat incorrect — c'est-à-dire quand le résultat est trop grand ou trop petit pour tenir dans le registre en tant qu'entier signé.

La différence entre CF et OF :
- **CF** : débordement du point de vue **non signé** (le résultat dépasse la capacité en tant qu'entier positif).
- **OF** : débordement du point de vue **signé** (le résultat sort de l'intervalle [-2^(n-1), 2^(n-1)-1]).

```nasm
; Exemple OF = 1 : addition de deux positifs qui donne un négatif
mov rax, 0x7FFFFFFFFFFFFFFF   ; max int64 signé = 9223372036854775807
add rax, 1                    ; résultat = 0x8000000000000000 → négatif ! OF = 1

; Exemple OF = 1 : addition de deux négatifs qui donne un positif
mov rax, 0x8000000000000000   ; min int64 = -9223372036854775808
add rax, rax                  ; 2 * min = débordement → OF = 1

; Exemple OF = 0 : opération signée correcte
mov rax, 100
add rax, 200    ; 300, dans les limites → OF = 0
```

**Quand OF est mis à 1 :**
- Addition : (+) + (+) = (-) ou (-) + (-) = (+)
- Soustraction : (+) - (-) = (-) ou (-) - (+) = (+)

### PF — Parity Flag

PF est 1 si le **nombre de bits à 1** dans les 8 bits bas du résultat est **pair**. Rarement utilisé de nos jours (vestige des communications série). On le mentionne pour être complet.

### AF — Auxiliary Carry Flag

Retenue entre les bits 3 et 4. Utilisé pour l'arithmétique BCD (Binary Coded Decimal). Pratiquement jamais utilisé dans le code moderne.

### DF — Direction Flag (drapeau de direction)

Le **Direction Flag** contrôle la **direction** des instructions de chaîne (MOVS, CMPS, SCAS, LODS, STOS). Si DF = 0, les pointeurs avancent (de l'adresse basse vers l'adresse haute). Si DF = 1, les pointeurs reculent.

```nasm
cld    ; CLeaD = met DF à 0 (direction "forward")
std    ; SeT Direction = met DF à 1 (direction "backward")
```

En pratique, la convention d'appel Linux stipule que DF doit être 0 à l'entrée et à la sortie de toute fonction. Si tu utilises `std`, remets `cld` avant de retourner.

### Tableau de synthèse : quand chaque flag est activé

```
┌──────┬──────────────────────────────────────────────────────────────────┐
│ FLAG │ Mis à 1 quand...                                                  │
├──────┼──────────────────────────────────────────────────────────────────┤
│  CF  │ Retenue (add) ou emprunt (sub) non signé                         │
│      │ Bit sorti d'un décalage/rotation                                 │
├──────┼──────────────────────────────────────────────────────────────────┤
│  ZF  │ Le résultat est zéro                                              │
├──────┼──────────────────────────────────────────────────────────────────┤
│  SF  │ Le bit de poids fort du résultat est 1 (résultat "négatif")      │
├──────┼──────────────────────────────────────────────────────────────────┤
│  OF  │ Débordement signé : le résultat sort de la plage int signé       │
│      │ (+)+(+)=(-) ou (-)+(-)=(+) ou (+)-(-)=(-) ou (-)-(+)=(+)        │
├──────┼──────────────────────────────────────────────────────────────────┤
│  PF  │ Nombre pair de bits à 1 dans les 8 bits bas du résultat          │
├──────┼──────────────────────────────────────────────────────────────────┤
│  AF  │ Retenue entre bit 3 et bit 4 (BCD, rarissime)                    │
├──────┼──────────────────────────────────────────────────────────────────┤
│  DF  │ Direction des string ops (1 = backward). Modifié par STD/CLD     │
└──────┴──────────────────────────────────────────────────────────────────┘
```

---

## 9. Les instructions arithmétiques et logiques

### Addition : ADD

```nasm
add destination, source
; destination = destination + source
; Flags affectés : CF, ZF, SF, OF, PF, AF
```

```nasm
add rax, rbx       ; RAX = RAX + RBX
add rax, 10        ; RAX = RAX + 10
add [rbx], rax     ; mem[RBX] = mem[RBX] + RAX
```

### Addition avec retenue : ADC (Add with Carry)

```nasm
adc destination, source
; destination = destination + source + CF
```

`ADC` est utilisé pour les additions **multi-précision** (sur des entiers plus grands que 64 bits). Si tu as un nombre de 128 bits dans `RDX:RAX` (RDX = 64 bits hauts, RAX = 64 bits bas) et un autre dans `RCX:RBX`, pour les additionner :

```nasm
add rax, rbx    ; additionne les parties basses, CF = retenue éventuelle
adc rdx, rcx    ; additionne les parties hautes + la retenue de l'addition précédente
```

### Soustraction : SUB

```nasm
sub destination, source
; destination = destination - source
; Flags affectés : CF (=1 si emprunt), ZF, SF, OF, PF, AF
```

```nasm
sub rax, rbx     ; RAX = RAX - RBX
sub rax, 5       ; RAX = RAX - 5
```

### Soustraction avec emprunt : SBB (Subtract with Borrow)

```nasm
sbb destination, source
; destination = destination - source - CF
```

Analogue à ADC pour la soustraction multi-précision.

### Négation : NEG

```nasm
neg destination
; destination = -destination = 0 - destination (complément à deux)
; CF = 0 si destination était 0, 1 sinon
```

```nasm
mov rax, 5
neg rax    ; RAX = -5 = 0xFFFFFFFFFFFFFFFB
```

### Incrémentation / décrémentation : INC, DEC

```nasm
inc destination    ; destination = destination + 1
dec destination    ; destination = destination - 1
```

**Important :** `INC` et `DEC` mettent à jour ZF, SF, OF, PF, AF mais **PAS CF**. C'est voulu pour permettre des incrémentations dans des boucles sans perturber un CF issu d'une opération précédente. Mais c'est un piège classique en ASM !

### Multiplication non signée : MUL

```nasm
mul source
```

`MUL` multiplie implicitement `RAX` (ou une partie de RAX selon la taille) par `source`. Le résultat, qui peut faire **le double de la taille** des opérandes, est stocké dans `RDX:RAX` (RDX = partie haute, RAX = partie basse).

```nasm
; Multiplication 64 bits × 64 bits → 128 bits dans RDX:RAX
mov rax, 0xFFFFFFFFFFFFFFFF
mov rbx, 2
mul rbx    ; RDX:RAX = 0xFFFFFFFFFFFFFFFF * 2 = 0x1FFFFFFFFFFFFFFFE
           ; RAX = 0xFFFFFFFFFFFFFFFE, RDX = 0x0000000000000001
```

Pour des tailles inférieures :
- `mul bl` (8 bits) : AX = AL * BL
- `mul bx` (16 bits) : DX:AX = AX * BX
- `mul ebx` (32 bits) : EDX:EAX = EAX * EBX
- `mul rbx` (64 bits) : RDX:RAX = RAX * RBX

### Multiplication signée : IMUL

```nasm
imul source                     ; même que MUL mais signé, résultat dans RDX:RAX
imul destination, source        ; destination = destination * source
imul destination, source, imm   ; destination = source * immédiat
```

`IMUL` a plusieurs formes. La forme à deux ou trois opérandes est très pratique car le résultat tient dans un seul registre (si pas de débordement).

```nasm
imul rax, rbx          ; RAX = RAX * RBX (signé, résultat sur 64 bits)
imul rcx, rdx, 5       ; RCX = RDX * 5
imul eax, [rbx + 4]    ; EAX = EAX * mem[RBX+4]
```

### Division non signée : DIV

```nasm
div source
```

Divise `RDX:RAX` par `source`. Le quotient va dans `RAX`, le reste dans `RDX`.

```nasm
; Diviser 100 par 7
xor rdx, rdx    ; TOUJOURS vider RDX avant une division 64 bits !
mov rax, 100
mov rbx, 7
div rbx         ; RAX = 14 (quotient), RDX = 2 (reste)
```

**Attention :** si RDX n'est pas mis à zéro avant une division non signée, le dividende est `RDX:RAX` (128 bits). Si le quotient ne rentre pas dans RAX (64 bits), une exception **#DE (Division Error)** est levée et ton programme crash.

### Division signée : IDIV

Divise `RDX:RAX` (signé) par `source`. Il faut d'abord étendre le signe de RAX vers RDX avec `CQO`.

```nasm
; Diviser -100 par 7
mov rax, -100
cqo             ; CQO = Convert Qword to Octaword : signe-étend RAX vers RDX:RAX
                ; si RAX < 0 → RDX = 0xFFFFFFFFFFFFFFFF
                ; si RAX ≥ 0 → RDX = 0
mov rbx, 7
idiv rbx        ; RAX = -14 (quotient), RDX = -2 (reste)
```

Les extensions de signe pour les autres tailles :
- `cbw` : étend AL (8b) vers AX (16b)
- `cwde` : étend AX (16b) vers EAX (32b)
- `cdq` : étend EAX (32b) vers EDX:EAX (64b)
- `cqo` : étend RAX (64b) vers RDX:RAX (128b)

### Opérations logiques : AND, OR, XOR, NOT

Ces instructions opèrent **bit à bit** sur leurs opérandes.

**AND** : les deux bits doivent être 1 pour que le résultat soit 1.
```nasm
and rax, rbx       ; RAX = RAX AND RBX (bit à bit)
and rax, 0xFF      ; RAX = RAX AND 0xFF → ne garde que les 8 bits bas (masque)
; Flags : CF=0, OF=0, ZF/SF/PF selon le résultat
```

**OR** : au moins un des deux bits doit être 1.
```nasm
or rax, rbx        ; RAX = RAX OR RBX
or rax, 0x80       ; met le bit 7 à 1 sans toucher aux autres
```

**XOR** : les deux bits doivent être différents pour que le résultat soit 1.
```nasm
xor rax, rbx       ; RAX = RAX XOR RBX
xor rax, rax       ; astuce classique : RAX = 0 (plus rapide que mov rax, 0)
                   ; car en binaire : X XOR X = 0 toujours
```

`XOR` avec soi-même pour mettre à zéro est une idiome ASM très répandue. Elle génère une instruction plus courte que `mov rax, 0` car l'immédiat 0 n'a pas besoin d'être encodé.

**NOT** : inverse tous les bits (complément à un).
```nasm
not rax    ; RAX = bitwise NOT de RAX (tous les 0 deviennent 1 et vice-versa)
           ; Note : NOT ne modifie PAS les flags !
```

### TEST

`TEST` effectue un AND bit-à-bit entre deux opérandes **sans modifier les opérandes**, en ne mettant à jour que les flags. C'est l'analogue de `CMP` pour la comparaison, mais pour les tests de bits.

```nasm
test rax, rax       ; effectue RAX AND RAX, met ZF=1 si RAX=0
                    ; idiomatic : tester si un registre est nul
test rax, 1         ; teste si le bit 0 de RAX est à 1 (ZF=0 si oui)
test al, 0x0F       ; teste si les 4 bits bas de AL sont tous nuls
```

`test rax, rax` suivi de `jz label` est le moyen le plus idiomatique de tester si un registre est nul.

---

## 10. Décalage et rotation

Les opérations de décalage déplacent les bits d'un registre vers la gauche ou la droite. Elles sont fondamentales pour la manipulation de bits, la multiplication/division rapide par des puissances de 2, et la cryptographie.

### SHL / SAL — Shift Left (décalage logique gauche)

```nasm
shl destination, count    ; décale les bits vers la gauche de 'count' positions
                          ; les bits qui sortent à gauche vont dans CF (dernier sorti)
                          ; les bits qui entrent à droite sont des 0
```

`SAL` (Shift Arithmetic Left) est identique à `SHL`. Un décalage gauche de n = multiplication par 2^n (non signé).

```nasm
mov rax, 1
shl rax, 3    ; RAX = 1 << 3 = 8 (= 1 * 2^3)
              ; CF = dernier bit sorti (ici 0)

mov rax, 0b00001010
shl rax, 2    ; RAX = 0b00101000 = 40
              ; Bits sortis : 00, donc CF = 0

; Avec CL comme compteur
mov rcx, 4
shl rax, cl   ; le compteur peut être dans CL (pas ECX, pas RCX — seulement CL !)
```

**Flags :** CF = dernier bit sorti. OF = 1 si le bit de signe a changé (pour un décalage de 1). ZF, SF, PF selon le résultat.

### SHR — Shift Right Logical (décalage logique droit)

```nasm
shr destination, count    ; décale vers la droite
                          ; les bits qui sortent à droite vont dans CF
                          ; les bits qui entrent à gauche sont des 0
```

SHR = division par 2^n pour les **non signés**.

```nasm
mov rax, 8
shr rax, 3    ; RAX = 8 >> 3 = 1
              ; équivalent à 8 / 8

mov rax, 0b11110000
shr rax, 2    ; RAX = 0b00111100
```

### SAR — Shift Arithmetic Right (décalage arithmétique droit)

```nasm
sar destination, count    ; décale vers la droite
                          ; les bits qui entrent à gauche = copie du bit de signe
```

`SAR` préserve le signe. C'est la division par 2^n pour les **signés**.

```nasm
mov rax, -8      ; RAX = 0xFFFFFFFFFFFFFFF8
sar rax, 1       ; RAX = -4 = 0xFFFFFFFFFFFFFFFC (le bit de signe 1 se propage)

mov rax, -7
sar rax, 1       ; RAX = -4 (arrondi vers -∞, pas vers 0 — différent de la division C !)
```

**Différence cruciale SHR vs SAR pour les nombres négatifs :**

```nasm
; SHR de -1 (0xFFFF...FFFF) :
mov rax, -1
shr rax, 1    ; RAX = 0x7FFF...FFFF = +9223372036854775807 (FAUX pour une division signée !)

; SAR de -1 :
mov rax, -1
sar rax, 1    ; RAX = -1 (le bit de signe se propage, reste -1 car -1/2 = -1 arrondi)
              ; en réalité SAR de -1 par 1 donne -1, pas 0, car arrondi vers -∞
```

### ROL / ROR — Rotation (gauche / droite)

La rotation est comme le décalage, mais les bits qui "sortent" d'un côté **réapparaissent de l'autre côté**. Aucun bit n'est perdu.

```nasm
rol destination, count    ; rotation à gauche
ror destination, count    ; rotation à droite
```

```nasm
mov al, 0b10110001
rol al, 1    ; AL = 0b01100011 (le bit qui sort à gauche revient à droite)
             ; CF = dernier bit sorti = 1

mov al, 0b10110001
ror al, 1    ; AL = 0b11011000 (le bit qui sort à droite revient à gauche)
             ; CF = dernier bit sorti (le bit 0 original = 1)
```

### RCL / RCR — Rotation through Carry (avec CF)

`RCL` et `RCR` incluent CF dans la rotation. C'est comme si CF était un bit supplémentaire qui fait partie du registre pour la rotation.

```nasm
; Pour un byte AL et CF, on a une rotation de 9 bits : [CF | AL]
rcl al, 1    ; {CF, AL} = {AL[7], CF, AL[6:1]} — rotation gauche de {CF,AL}
rcr al, 1    ; {AL, CF} = {AL[0], AL[7:1], CF} — rotation droite de {CF,AL}
```

Ces opérations sont utilisées pour les décalages multi-précision.

---

## 11. Comparaisons et sauts

### CMP — Comparer

```nasm
cmp operande1, operande2
; Effectue operande1 - operande2
; Met à jour les flags selon le résultat
; NE MODIFIE PAS les opérandes
```

`CMP` est identique à `SUB` mais sans stocker le résultat. On utilise ensuite les flags pour sauter conditionnel.

```nasm
cmp rax, rbx     ; "compare RAX et RBX" = effectue RAX - RBX
cmp rax, 0       ; compare RAX avec 0
cmp byte [rbx], 10  ; compare un octet en mémoire avec 10
```

### Les sauts inconditionnels : JMP

```nasm
jmp label        ; saute toujours à 'label'
jmp rax          ; saute à l'adresse contenue dans RAX (saut indirect)
jmp [rax]        ; saute à l'adresse stockée à l'adresse dans RAX
```

`JMP` modifie `RIP` directement. L'exécution reprend à `label`.

### Les sauts conditionnels : Jcc

Les sauts conditionnels (`Jcc`) lisent les flags et sautent ou non selon une condition. Ils sont toujours utilisés après `CMP` ou `TEST`.

Voici la liste complète avec les conditions en termes de flags et la signification humaine :

#### Pour les comparaisons non signées (unsigned)

```
┌────────────┬──────────────────────────────────────────────────────────┐
│ Instruction│ Condition / signification                                 │
├────────────┼──────────────────────────────────────────────────────────┤
│ JE  / JZ   │ ZF=1       → "jump if equal" / "jump if zero"           │
│ JNE / JNZ  │ ZF=0       → "jump if not equal" / "not zero"           │
│ JB  / JC   │ CF=1       → "jump if below" (A < B, non signé)         │
│ JAE / JNC  │ CF=0       → "jump if above or equal" (A >= B, non sig.)│
│ JA         │ CF=0 & ZF=0→ "jump if above" (A > B, non signé)         │
│ JBE        │ CF=1 | ZF=1→ "jump if below or equal" (A <= B, non sig.)│
└────────────┴──────────────────────────────────────────────────────────┘
```

#### Pour les comparaisons signées (signed)

```
┌────────────┬──────────────────────────────────────────────────────────┐
│ Instruction│ Condition / signification                                 │
├────────────┼──────────────────────────────────────────────────────────┤
│ JE  / JZ   │ ZF=1       → "jump if equal"                            │
│ JNE / JNZ  │ ZF=0       → "jump if not equal"                        │
│ JL  / JNGE │ SF≠OF      → "jump if less" (A < B, signé)             │
│ JGE / JNL  │ SF=OF      → "jump if greater or equal" (A >= B, signé)│
│ JG  / JNLE │ ZF=0 & SF=OF → "jump if greater" (A > B, signé)        │
│ JLE / JNG  │ ZF=1 | SF≠OF → "jump if less or equal" (A <= B, signé) │
└────────────┴──────────────────────────────────────────────────────────┘
```

#### Tests de flags individuels

```
┌────────────┬──────────────────────────┐
│ JS         │ SF=1 → résultat négatif  │
│ JNS        │ SF=0 → résultat positif  │
│ JO         │ OF=1 → overflow signé    │
│ JNO        │ OF=0 → pas d'overflow    │
│ JP / JPE   │ PF=1 → parité paire      │
│ JNP / JPO  │ PF=0 → parité impaire    │
│ JRCXZ      │ RCX=0 → test registre    │
└────────────┴──────────────────────────┘
```

### Pourquoi JL utilise SF≠OF et non seulement SF ?

C'est la question qui déroute beaucoup. Voici l'explication :

Quand tu fais `cmp rax, rbx` (= `rax - rbx`) :
- Si le résultat est négatif **sans overflow**, SF=1 et OF=0 → SF≠OF → RAX < RBX. Correct.
- Si le résultat est positif **avec overflow** (ex: INT_MIN - INT_MAX = positif → OF=1, SF=0) → SF≠OF → RAX < RBX. Correct car l'overflow a inversé le signe.
- Si le résultat est négatif **avec overflow** (ex: INT_MAX - INT_MIN = négatif → OF=1, SF=1) → SF=OF → RAX >= RBX. Correct.

En résumé : quand il y a overflow, le signe du résultat est *inversé* par rapport à la vraie comparaison. La condition SF≠OF compense automatiquement cet inversement.

### Exemple complet : boucle avec comparaison

```nasm
; Compter de 0 à 9
section .text
global _start
_start:
    mov rcx, 0       ; compteur = 0

.boucle:
    ; ... faire quelque chose avec RCX ...
    inc rcx          ; compteur++
    cmp rcx, 10      ; comparer avec 10
    jl .boucle       ; si rcx < 10, recommencer (signé)
    ; ou jb .boucle  ; si on utilise non signé (ici pareil car valeurs positives)

    ; sortie du programme
    mov rax, 60
    xor rdi, rdi
    syscall
```

### SETcc — Set Byte on Condition

`SETcc` met un octet à 1 si la condition est vraie, à 0 sinon. Utile pour convertir une condition en valeur numérique sans branchement.

```nasm
cmp rax, rbx
sete al     ; AL = 1 si RAX == RBX, 0 sinon
setg cl     ; CL = 1 si RAX > RBX (signé), 0 sinon
setb dl     ; DL = 1 si RAX < RBX (non signé), 0 sinon
```

---

## 12. La pile (stack) et les frames

### Concept fondamental de la pile

La pile (stack) est une structure de données de type **LIFO** (Last In, First Out — le dernier entré est le premier sorti). En x86-64, la pile **croît vers le bas** : quand on empile quelque chose, l'adresse diminue.

`RSP` (Register Stack Pointer) pointe **toujours vers le sommet actuel de la pile**, c'est-à-dire le dernier élément empilé (la donnée valide la plus récente).

```
Mémoire (adresses) :
         État initial
         RSP = 0x7FFF_0020
         ┌─────────────────┐ 0x7FFF_0028
         │ (valeur existe) │
         ├─────────────────┤ 0x7FFF_0020  ← RSP (sommet)
         │ données valides │
         ├─────────────────┤ 0x7FFF_0018
         │                 │
         ...
```

### PUSH — empiler une valeur

```nasm
push rax    ; RSP -= 8, puis mem[RSP] = RAX
push 42     ; RSP -= 8, puis mem[RSP] = 42
```

En détail :

```
Avant PUSH RAX (RAX=0x1234) :          Après PUSH RAX :
RSP = 0x7FFF_0020                       RSP = 0x7FFF_0018
┌─────────────────┐ 0x7FFF_0020        ┌─────────────────┐ 0x7FFF_0020
│ ancien sommet   │ ← RSP              │ ancien sommet   │
├─────────────────┤ 0x7FFF_0018        ├─────────────────┤ 0x7FFF_0018
│ (libre)         │                    │    0x1234        │ ← RSP (nouveau sommet)
└─────────────────┘                    └─────────────────┘
```

### POP — dépiler une valeur

```nasm
pop rax    ; RAX = mem[RSP], puis RSP += 8
```

```
Avant POP RAX :                         Après POP RAX (RAX = 0x1234) :
RSP = 0x7FFF_0018                       RSP = 0x7FFF_0020
┌─────────────────┐ 0x7FFF_0020        ┌─────────────────┐ 0x7FFF_0020
│ ancien sommet   │                    │ ancien sommet   │ ← RSP
├─────────────────┤ 0x7FFF_0018        ├─────────────────┤ 0x7FFF_0018
│    0x1234        │ ← RSP             │    0x1234        │ (toujours là mais non valide)
└─────────────────┘                    └─────────────────┘
```

Note : les données "dépilées" restent en mémoire mais sont considérées comme invalides (elles seront écrasées par le prochain push).

### CALL et RET : les appels de fonctions

`CALL label` fait deux choses :
1. Empile l'adresse de l'instruction suivante (l'adresse de retour).
2. Saute à `label`.

`RET` fait l'inverse :
1. Dépile l'adresse de retour dans RIP.
2. Saute à cette adresse.

```nasm
; Dans le code appelant :
call ma_fonction    ; empile adresse_retour, saute à ma_fonction

; Dans ma_fonction :
ma_fonction:
    ; ... code de la fonction ...
    ret             ; dépile adresse_retour, y retourne
```

En termes de pile, `call` est équivalent à :

```nasm
; CALL label est équivalent à :
push rip_suivant    ; (RIP = adresse de l'instruction après le call)
jmp label
```

Et `ret` est équivalent à :

```nasm
; RET est équivalent à :
pop rip             ; ou plus précisément :
                    ; RIP = mem[RSP], RSP += 8
```

### La stack frame (cadre de pile)

Quand une fonction est appelée, elle crée sa propre **frame** sur la pile. Cette frame contient :
1. L'adresse de retour (empilée par `call`)
2. L'ancienne valeur de `RBP` (sauvegardée)
3. Les variables locales de la fonction
4. Les registres sauvegardés

La **convention** est d'utiliser `RBP` comme pointeur de **base** de la frame, qui ne bougera pas pendant toute la durée de la fonction (contrairement à RSP qui peut bouger). Cela permet d'accéder aux variables locales et aux arguments via des offsets fixes par rapport à RBP.

**Prologue standard d'une fonction :**

```nasm
ma_fonction:
    push rbp           ; sauvegarde l'ancien RBP de l'appelant
    mov rbp, rsp       ; RBP = RSP (base de notre frame)
    sub rsp, N         ; réserve N octets pour les variables locales
                       ; (N doit être un multiple de 16 pour l'alignement)
    ; ... corps de la fonction ...
```

**Épilogue standard (avant ret) :**

```nasm
    mov rsp, rbp       ; restaure RSP (libère les variables locales)
    pop rbp            ; restaure l'ancien RBP
    ret
```

Ou plus court avec `LEAVE` :

```nasm
    leave              ; équivalent à : mov rsp, rbp / pop rbp
    ret
```

### Visualisation de la pile pendant un appel de fonction

```
Avant l'appel à ma_fonction() depuis _start :

    Mémoire              RSP → sommet de la pile
    
Pendant l'appel (après CALL) :

    ┌─────────────────────┐ adresses hautes
    │  ...                │
    │  [frame de _start]  │
    │  variables locales  │
    │  de _start          │
    ├─────────────────────┤
    │  adresse de retour  │ ← RSP au moment du CALL
    ├─────────────────────┤
    │  ancien RBP         │ ← RSP après "push rbp"
    ├─────────────────────┤ ← RBP après "mov rbp, rsp"
    │  variable locale 1  │ [rbp - 8]
    │  variable locale 2  │ [rbp - 16]
    │  ...                │
    ├─────────────────────┤ ← RSP après "sub rsp, N"
    │  (espace libre)     │
    └─────────────────────┘ adresses basses
```

### Accès aux variables locales et aux arguments

```nasm
ma_fonction:
    push rbp
    mov rbp, rsp
    sub rsp, 32         ; réserve 32 octets (4 qwords)

    ; Stocker des variables locales :
    mov qword [rbp - 8],  rdi    ; 1ère var locale = 1er argument
    mov qword [rbp - 16], rsi    ; 2ème var locale = 2ème argument
    mov qword [rbp - 24], 42     ; 3ème var locale = constante

    ; Lire les variables locales :
    mov rax, [rbp - 8]
    add rax, [rbp - 16]

    leave
    ret
```

### L'alignement de la pile sur 16 octets

La **System V AMD64 ABI** (la convention d'appel de Linux) requiert que `RSP` soit aligné sur 16 octets **au moment du `call`**. En pratique, RSP doit être ≡ 8 (mod 16) juste avant un `call` (car le `call` va empiler 8 octets et RSP sera alors aligné sur 16).

Pourquoi ? Les instructions SSE et AVX travaillent sur des données 16 ou 32 octets alignées. Si tu appelles une fonction qui utilise ces instructions avec une pile non alignée, tu peux obtenir une exception de protection.

Si tu oublies l'alignement, des bibliothèques comme la `libc` crasheront de façon inexplicable.

Règle simple : dans le prologue de ta fonction `_start`, fais `sub rsp, 8` si tu vas appeler des fonctions et que tu n'as pas fait de `push` impair auparavant.

---

## 13. La convention d'appel

### Qu'est-ce que la convention d'appel ?

Une convention d'appel (calling convention) est un ensemble de règles qui définissent comment les fonctions se passent les arguments, où elles retournent leur résultat, et quels registres elles sont autorisées à modifier. Sans convention partagée, une fonction écrite en ASM ne pourrait pas être appelée depuis du C (et vice-versa), car l'appelant et l'appelé auraient des attentes différentes.

En Linux 64 bits, la convention utilisée est la **System V AMD64 ABI**.

### Passage des arguments

Les 6 premiers arguments entiers/pointeurs sont passés dans les registres dans cet ordre :

```
1er argument : RDI
2ème argument : RSI
3ème argument : RDX
4ème argument : RCX
5ème argument : R8
6ème argument : R9
```

Si la fonction a plus de 6 arguments, les suivants sont passés **sur la pile** (dans l'ordre inverse, empilés avant le `call`).

Pour les arguments **flottants**, on utilise les registres `XMM0` à `XMM7`.

Exemple en C et son équivalent côté appel ASM :

```c
// En C :
long ma_fonction(long a, long b, long c);
long resultat = ma_fonction(10, 20, 30);
```

```nasm
; En ASM (côté appelant) :
mov rdi, 10     ; 1er argument
mov rsi, 20     ; 2ème argument
mov rdx, 30     ; 3ème argument
call ma_fonction
; après le call, le résultat est dans RAX
```

### Valeur de retour

- Entiers et pointeurs : retournés dans **RAX** (et RDX pour les valeurs de 128 bits).
- Flottants : retournés dans **XMM0**.
- Structures > 16 octets : un pointeur vers un espace alloué par l'appelant est passé en RDI comme argument "caché".

### Registres caller-saved vs callee-saved

C'est la distinction la plus importante pour éviter les bugs :

**Caller-saved** (sauvegardés par l'appelant, "scratch registers") : ces registres peuvent être **librement modifiés** par la fonction appelée. Si tu as des valeurs importantes dedans avant un `call`, tu dois les sauvegarder toi-même (sur la pile ou dans d'autres registres).

```
RAX, RCX, RDX, RSI, RDI, R8, R9, R10, R11
```

**Callee-saved** (sauvegardés par l'appelé, "preserved registers") : si une fonction utilise ces registres, elle **doit** les sauvegarder (push au début) et les restaurer (pop à la fin). L'appelant peut compter sur le fait que ces registres ont la même valeur avant et après l'appel.

```
RBX, RBP, R12, R13, R14, R15
```

`RSP` a un statut spécial : la fonction peut le modifier (pour créer sa frame), mais doit le restaurer exactement avant `ret`.

### Tableau récapitulatif de la convention d'appel

```
┌───────────────────────────────────────────────────────────────────────┐
│                    System V AMD64 ABI (Linux)                         │
├─────────┬────────────────┬──────────────────────────────────────────── ┤
│Registre │ Rôle           │ Préservé par l'appelé ?                     │
├─────────┼────────────────┼────────────────────────────────────────────┤
│ RAX     │ Valeur retour  │ NON (caller-saved)                          │
│ RBX     │ Générique      │ OUI (callee-saved)                          │
│ RCX     │ 4ème argument  │ NON (caller-saved)                          │
│ RDX     │ 3ème argument  │ NON (caller-saved)                          │
│ RSI     │ 2ème argument  │ NON (caller-saved)                          │
│ RDI     │ 1er argument   │ NON (caller-saved)                          │
│ RBP     │ Base frame     │ OUI (callee-saved)                          │
│ RSP     │ Sommet pile    │ OUI (restauré avant ret)                    │
│ R8      │ 5ème argument  │ NON (caller-saved)                          │
│ R9      │ 6ème argument  │ NON (caller-saved)                          │
│ R10     │ Scratch        │ NON (caller-saved)                          │
│ R11     │ Scratch        │ NON (caller-saved)                          │
│ R12-R15 │ Générique      │ OUI (callee-saved)                          │
└─────────┴────────────────┴────────────────────────────────────────────┘
```

### Exemple complet : une fonction avec la convention d'appel

```nasm
; Fonction : long additionner(long a, long b)
; Arguments : RDI = a, RSI = b
; Retour : RAX = a + b
additionner:
    push rbp
    mov rbp, rsp
    ; Pas de variables locales, pas de sub rsp nécessaire ici

    ; Calcul
    mov rax, rdi    ; RAX = a
    add rax, rsi    ; RAX = a + b

    ; Épilogue
    pop rbp
    ret

; Appelant :
_start:
    mov rdi, 15     ; a = 15
    mov rsi, 27     ; b = 27
    call additionner
    ; RAX = 42
```

---

## 14. Les appels système (syscall)

### Qu'est-ce qu'un syscall ?

Le noyau Linux gère les ressources matérielles (clavier, écran, fichiers, réseau...). Un programme en espace utilisateur ne peut pas accéder directement à ces ressources — il doit **demander au noyau** de le faire pour lui. Ce mécanisme s'appelle un **appel système** (system call ou syscall).

En x86-64 Linux, l'instruction `syscall` déclenche une transition du mode utilisateur (Ring 3) vers le mode noyau (Ring 0). Le noyau exécute la fonction demandée puis rend la main au programme utilisateur.

### Convention d'appel des syscalls Linux

Les syscalls ont leur propre convention, **différente** de la convention d'appel C :

```
Numéro du syscall : RAX
1er argument      : RDI
2ème argument     : RSI
3ème argument     : RDX
4ème argument     : R10   (ATTENTION : pas RCX comme en C !)
5ème argument     : R8
6ème argument     : R9
Valeur de retour  : RAX   (négatif si erreur : -errno)
```

**Registres détruits par syscall** : `RCX` et `R11` sont **systématiquement écrasés** par l'instruction `syscall` (le noyau les utilise en interne). Si tu en as besoin après un syscall, sauvegarde-les avant.

### Numéros de syscalls courants (Linux x86-64)

```
┌──────┬─────────────┬───────────────────────────────────────────────┐
│  RAX │ Nom         │ Description                                    │
├──────┼─────────────┼───────────────────────────────────────────────┤
│  0   │ read        │ Lire depuis un fd                              │
│  1   │ write       │ Écrire vers un fd                              │
│  2   │ open        │ Ouvrir un fichier                              │
│  3   │ close       │ Fermer un fd                                   │
│  9   │ mmap        │ Mapper de la mémoire                           │
│  11  │ munmap      │ Dé-mapper de la mémoire                        │
│  12  │ brk         │ Modifier la limite du heap                     │
│  57  │ fork        │ Créer un processus fils                        │
│  59  │ execve      │ Exécuter un programme                          │
│  60  │ exit        │ Terminer le processus                          │
│  61  │ wait4       │ Attendre un processus fils                     │
│  231 │ exit_group  │ Terminer tous les threads du groupe            │
└──────┴─────────────┴───────────────────────────────────────────────┘
```

La liste complète est dans `/usr/include/asm/unistd_64.h` ou sur le site `syscall.sh`.

### Exemple : écrire sur stdout

```nasm
; write(1, message, 13)
; RAX = 1 (numéro syscall write)
; RDI = 1 (fd : STDOUT = 1)
; RSI = adresse du message
; RDX = longueur du message

section .data
    message db "Hello, World!", 10   ; 13 octets + newline
    msg_len equ $ - message          ; = 14

section .text
global _start
_start:
    mov rax, 1              ; syscall write
    mov rdi, 1              ; fd = stdout
    lea rsi, [rel message]  ; adresse du message
    mov rdx, msg_len        ; longueur
    syscall

    ; exit(0)
    mov rax, 60             ; syscall exit
    xor rdi, rdi            ; code de retour = 0
    syscall
```

### Exemple : lire depuis stdin

```nasm
; read(0, buffer, 256)
section .bss
    buffer resb 256

section .text
global _start
_start:
    mov rax, 0              ; syscall read
    mov rdi, 0              ; fd = stdin
    lea rsi, [rel buffer]   ; buffer de destination
    mov rdx, 256            ; nombre max d'octets à lire
    syscall
    ; RAX = nombre d'octets réellement lus (ou -errno si erreur)
```

### Exemple : ouvrir un fichier et le lire

```nasm
section .data
    nom_fichier db "/etc/hostname", 0    ; chemin null-terminé

section .bss
    buffer resb 256
    fd resq 1

section .text
global _start
_start:
    ; open("/etc/hostname", O_RDONLY=0)
    mov rax, 2
    lea rdi, [rel nom_fichier]
    mov rsi, 0              ; O_RDONLY
    mov rdx, 0              ; mode (ignoré pour O_RDONLY)
    syscall
    mov [rel fd], rax       ; sauvegarde le file descriptor

    ; read(fd, buffer, 256)
    mov rax, 0
    mov rdi, [rel fd]
    lea rsi, [rel buffer]
    mov rdx, 256
    syscall

    ; write(1, buffer, bytes_lus)
    mov rdx, rax            ; RAX = bytes lus, on réutilise comme longueur
    mov rax, 1
    mov rdi, 1
    lea rsi, [rel buffer]
    syscall

    ; close(fd)
    mov rax, 3
    mov rdi, [rel fd]
    syscall

    ; exit(0)
    mov rax, 60
    xor rdi, rdi
    syscall
```

### Gestion des erreurs

Si un syscall échoue, `RAX` contient une valeur **négative** correspondant à `-errno`. Les codes d'erreur sont définis dans `/usr/include/errno.h`. Par exemple, `ENOENT = 2`, donc si un fichier n'existe pas, `open` retourne `-2`.

```nasm
    syscall
    test rax, rax        ; teste si RAX < 0 (bit de signe)
    js .erreur           ; saute si négatif (SF=1)
    ; ... succès ...
.erreur:
    neg rax              ; RAX = -errno → errno
    ; ... gérer l'erreur ...
```

---

## 15. Les arguments de ligne de commande

### Comment le noyau passe les arguments

Quand tu lances un programme comme `./monprog arg1 arg2`, le noyau organise la pile **avant** que `_start` soit appelé. Il y empile les informations suivantes :

```
┌────────────────────────────┐ ← RSP au démarrage de _start
│ argc (qword)               │  = nombre d'arguments (incluant le nom du programme)
├────────────────────────────┤
│ argv[0] (pointeur)         │  → chaîne "./monprog\0"
│ argv[1] (pointeur)         │  → chaîne "arg1\0"
│ argv[2] (pointeur)         │  → chaîne "arg2\0"
│ NULL    (qword = 0)        │  fin du tableau argv
├────────────────────────────┤
│ envp[0] (pointeur)         │  → "PATH=/usr/bin\0"
│ envp[1] (pointeur)         │  → "HOME=/root\0"
│ ...                        │
│ NULL    (qword = 0)        │  fin du tableau envp
└────────────────────────────┘
```

### Accès aux arguments dans _start

```nasm
section .text
global _start
_start:
    ; Au démarrage, RSP pointe vers argc
    pop rdi          ; RDI = argc (et RSP avance de 8)
    ; Maintenant RSP pointe vers argv[0]
    mov rsi, rsp     ; RSI = adresse de argv (tableau de pointeurs)

    ; argv[0] = [rsi]
    mov rax, [rsi]        ; RAX = pointeur vers le nom du programme

    ; argv[1] = [rsi + 8]
    cmp rdi, 2            ; y a-t-il au moins 1 argument (argc >= 2) ?
    jl .pas_dargument
    mov rbx, [rsi + 8]    ; RBX = pointeur vers argv[1]
    ; RBX pointe vers une chaîne null-terminée

.pas_dargument:
    ; ...

    mov rax, 60
    xor rdi, rdi
    syscall
```

### Convertir un argument chaîne en entier

Les arguments sont des **chaînes de caractères**, pas des entiers. Pour utiliser un nombre passé en argument, il faut le convertir.

```nasm
; Fonction : atoi basique (convertit une chaîne ASCII en entier)
; RDI = pointeur vers la chaîne
; Retourne RAX = entier
atoi_simple:
    xor rax, rax         ; résultat = 0
    xor rcx, rcx         ; rcx = chiffre courant

.boucle:
    movzx rcx, byte [rdi]   ; lire un byte
    test rcx, rcx            ; null terminator ?
    jz .fin

    sub rcx, '0'             ; convertir ASCII → valeur ('0'=48, '9'=57)
    cmp rcx, 9
    ja .fin                  ; si > 9, ce n'est pas un chiffre

    imul rax, rax, 10        ; résultat = résultat * 10
    add rax, rcx             ; résultat += chiffre
    inc rdi                  ; avancer dans la chaîne
    jmp .boucle

.fin:
    ret
```

---

## 16. Écrire et assembler un programme complet

### Structure d'un programme NASM

```nasm
; Commentaires avec point-virgule

section .data
    ; Données initialisées
    message db "Hello, World!", 10, 0
    msg_len equ $ - message

section .bss
    ; Données non initialisées
    buffer resb 1024

section .text
    global _start         ; rend _start visible pour le linker

_start:
    ; Point d'entrée du programme
    ; ... code ...

    mov rax, 60           ; exit syscall
    xor rdi, rdi          ; code 0
    syscall
```

### Assembler et lier avec NASM + ld

```bash
# Assembler : .asm → .o (fichier objet)
nasm -f elf64 programme.asm -o programme.o

# Lier : .o → exécutable
ld programme.o -o programme

# Exécuter
./programme
```

L'option `-f elf64` spécifie le format de sortie (ELF 64 bits pour Linux).

### Utiliser les fonctions de la libc

Pour utiliser `printf`, `malloc`, etc., on doit lier avec la libc :

```nasm
; On peut utiliser printf depuis NASM
extern printf

section .data
    fmt db "Valeur : %ld", 10, 0

section .text
global main

main:
    push rbp
    mov rbp, rsp
    sub rsp, 16          ; alignement

    lea rdi, [rel fmt]   ; 1er arg de printf = format string
    mov rsi, 42          ; 2ème arg = la valeur
    xor rax, rax         ; RAX = 0 car pas d'arg XMM (convention varargs)
    call printf

    xor rax, rax         ; valeur de retour de main = 0
    leave
    ret
```

```bash
# Compilation avec libc (utiliser gcc comme linker)
nasm -f elf64 prog.asm -o prog.o
gcc prog.o -o prog -no-pie    # ou avec -pie si code position-independent
```

### Position Independent Code (PIC) et RIP-relative addressing

Les exécutables modernes sont compilés avec **PIE** (Position Independent Executable) : le code peut être chargé à n'importe quelle adresse en mémoire (ASLR — Address Space Layout Randomization). Dans ce cas, les adresses sont calculées relativement à `RIP`.

En NASM, pour accéder à des données de façon position-indépendante :

```nasm
lea rdi, [rel message]    ; charge l'adresse de 'message' relativement à RIP
                          ; fonctionne peu importe où le programme est chargé
```

Sans `rel`, NASM génère une adresse absolue qui ne fonctionnera pas en mode PIE.

---

## 17. Débogage et reverse engineering

### GDB — GNU Debugger

GDB est l'outil principal pour déboguer des programmes en ASM (et en C).

**Lancer un programme sous GDB :**

```bash
gdb ./programme
```

**Commandes GDB essentielles :**

```
(gdb) run [args]         — lancer le programme
(gdb) break _start       — point d'arrêt au label _start
(gdb) break *0x401000    — point d'arrêt à l'adresse 0x401000
(gdb) stepi              — exécuter une instruction ASM
(gdb) nexti              — exécuter une instruction ASM (saute par-dessus les call)
(gdb) continue           — continuer jusqu'au prochain breakpoint
(gdb) info registers     — afficher tous les registres
(gdb) print $rax         — afficher RAX
(gdb) x/10x $rsp         — afficher 10 valeurs hexa à partir de RSP
(gdb) x/s $rdi           — afficher une chaîne à l'adresse dans RDI
(gdb) disassemble        — désassembler la fonction courante
(gdb) disassemble /r func — désassembler avec les octets bruts
(gdb) layout regs        — interface TUI avec registres
(gdb) layout asm         — interface TUI avec le code ASM
(gdb) set disassembly-flavor intel  — passer en syntaxe Intel
```

**Pour debugger en syntaxe Intel (fortement recommandé) :**

```bash
echo "set disassembly-flavor intel" >> ~/.gdbinit
```

**Afficher l'état des flags :**

```
(gdb) info registers eflags
(gdb) p $eflags
```

### GDB avec pwndbg ou peda

Des extensions de GDB existent pour améliorer l'interface de débogage en sécurité :

- **pwndbg** : (https://github.com/pwndbg/pwndbg) — très utilisé en CTF et pwn
- **peda** : Python Exploit Development Assistance for GDB
- **gef** : GDB Enhanced Features

Ces extensions affichent automatiquement les registres, la pile, le code désassemblé et les flags à chaque pas d'exécution.

### objdump — désassembler un binaire

```bash
# Désassembler toutes les sections de code
objdump -d programme

# Désassembler en syntaxe Intel
objdump -d -M intel programme

# Afficher aussi les octets bruts
objdump -d -M intel --show-raw-insn programme

# Afficher les sections et leur contenu
objdump -s programme

# Afficher les symboles
objdump -t programme
```

### readelf — analyser la structure ELF

```bash
readelf -h programme          # en-tête ELF
readelf -S programme          # sections
readelf -l programme          # segments (program headers)
readelf -s programme          # table des symboles
readelf -d programme          # dynamic linking info
```

### strace — tracer les syscalls

```bash
strace ./programme            # affiche tous les syscalls
strace -e trace=write,read ./programme  # filtrer par syscall
```

Extrêmement utile pour comprendre ce que fait un programme inconnu sans lire son code.

### ltrace — tracer les appels de bibliothèques

```bash
ltrace ./programme            # affiche tous les appels à la libc
```

### Radare2 / Ghidra / IDA Pro — reverse engineering avancé

**Radare2** (open source) : framework de reverse engineering complet.

```bash
r2 ./programme
> aaa             # analyser tout le binaire
> afl             # lister toutes les fonctions
> pdf @ main      # désassembler la fonction main
> VV              # mode graphe (visual mode)
> iz              # lister les chaînes de caractères
```

**Ghidra** (NSA, open source) : désassembleur + décompilateur graphique. Extrêmement puissant pour retrouver du pseudo-code C depuis un binaire.

**IDA Pro** (commercial) : la référence industrielle pour l'analyse de malwares.

### Comprendre un binaire inconnu : méthodologie

Quand tu reçois un binaire sans code source :

1. `file ./binaire` — quel type de fichier ? architecture ? stripped ?
2. `strings ./binaire` — chaînes lisibles (souvent révèle le but du programme)
3. `readelf -s ./binaire` — symboles (si pas stripped)
4. `objdump -d -M intel ./binaire` — désassembly complet
5. `strace ./binaire` — observer le comportement à l'exécution
6. `ltrace ./binaire` — appels de bibliothèques
7. Ouvrir dans Ghidra/IDA pour le décompilateur

---

## 18. Stack Buffer Overflow

### Qu'est-ce qu'un buffer overflow ?

Un **buffer overflow** (dépassement de tampon) se produit quand un programme écrit plus de données dans un buffer qu'il n'a de place pour les recevoir, débordant dans les zones mémoire adjacentes.

Sur la pile, un buffer overflow peut **écraser l'adresse de retour** sauvegardée par `call`. Comme `ret` va lire cette adresse et y sauter, si on la contrôle, on contrôle le flot d'exécution.

### Anatomie d'une vulnérabilité

Considérons ce code C (vulnérable) :

```c
#include <stdio.h>
#include <string.h>

void vulnerable(char *input) {
    char buffer[64];
    strcpy(buffer, input);    // PAS de vérification de longueur !
}

int main(int argc, char *argv[]) {
    vulnerable(argv[1]);
    return 0;
}
```

Regardons la stack frame de `vulnerable()` :

```
Haute mémoire
┌─────────────────────────────┐
│ arguments de main()         │
├─────────────────────────────┤
│ adresse de retour de main() │
├─────────────────────────────┤
│ ancien RBP de main          │
├─────────────────────────────┤ ← RBP de vulnerable
│ adresse de retour           │ ← ICI on veut rediriger l'exécution
│ (vers main, après l'appel)  │
├─────────────────────────────┤
│ ancien RBP de vulnerable    │
├─────────────────────────────┤ ← [rbp]
│  buffer[63]                 │
│  buffer[62]                 │
│  ...                        │
│  buffer[1]                  │
│  buffer[0]                  │ ← [rbp - 64] ← début du buffer
└─────────────────────────────┘
Basse mémoire
```

Si `strcpy` copie plus de 64 octets, il va écraser :
1. Le rembourrage éventuel (padding)
2. L'ancien RBP (8 octets)
3. L'adresse de retour (8 octets) ← **LA CIBLE**

### L'exploit classique : redirection d'exécution

```
Input : [64 octets de remplissage] [8 octets écrasant RBP] [adresse cible sur 8 octets]
```

Si on met comme "adresse cible" l'adresse d'une autre fonction (comme une fonction `win()` cachée, ou du shellcode placé dans le buffer), `ret` va y sauter.

### Shellcode

Un shellcode est du code machine (séquence d'octets) qui, quand exécuté, lance un shell ou effectue une action malveillante. L'objectif est d'en injecter dans un buffer vulnérable, puis de rediriger l'exécution vers lui.

Exemple conceptuel de shellcode minimal pour `execve("/bin/sh", NULL, NULL)` :

```nasm
; execve("/bin/sh", NULL, NULL)
; RAX = 59 (syscall execve)
; RDI = adresse de "/bin/sh"
; RSI = 0
; RDX = 0

xor rdx, rdx          ; rdx = 0
xor rsi, rsi          ; rsi = 0
lea rdi, [rip + 13]   ; rdi = adresse de la chaîne juste après ce code
push 59
pop rax
syscall
db "/bin/sh", 0
```

### Les protections modernes

Les systèmes modernes ont plusieurs protections contre les buffer overflows :

**1. ASLR (Address Space Layout Randomization)**
L'adresse de la pile, de la heap, et des bibliothèques est **aléatoire** à chaque exécution. On ne peut pas hardcoder une adresse de retour. Contournement : fuite d'adresse (info leak), brute force en 32 bits.

**2. NX / DEP (No-Execute / Data Execution Prevention)**
La pile et la heap sont marquées **non exécutables**. Injecter du shellcode et y sauter ne fonctionne plus. Contournement : **ROP** (Return-Oriented Programming).

**3. Stack Canary**
Le compilateur insère une valeur aléatoire ("canari") entre les variables locales et l'adresse de retour. Avant de retourner, la fonction vérifie que le canari n'a pas été modifié. Si oui → crash intentionnel. Contournement : fuite du canari.

**4. PIE (Position Independent Executable)**
Le programme lui-même est chargé à une adresse aléatoire. Contournement : fuite d'adresse du texte.

### ROP — Return-Oriented Programming

Avec NX actif, on ne peut plus exécuter de code injecté. Mais on peut **enchaîner des gadgets** : de petits bouts de code existants (dans le programme ou la libc) qui se terminent par `ret`.

Un gadget ROP ressemble à :

```nasm
pop rdi
ret
```

En contrôlant la pile, on enchaîne les gadgets pour faire des calculs arbitraires. Par exemple, pour appeler `execve("/bin/sh", 0, 0)` via ROP :

```
Adresse gadget "pop rdi; ret"
Adresse de la chaîne "/bin/sh"
Adresse gadget "pop rsi; ret"
0
Adresse gadget "pop rdx; ret"
0
Adresse gadget "pop rax; ret"
59
Adresse gadget "syscall; ret"
```

Chaque `ret` prend la prochaine adresse sur la pile et y saute. On "programme" ainsi en ASM sans injecter de code, en utilisant du code déjà présent.

**Outils ROP :**
- `ROPgadget --binary programme` — trouver les gadgets
- `pwntools` (Python) — framework d'exploitation
- `ropper` — autre outil de recherche de gadgets

---

## 19. Vers les programmes complexes

### Écrire un programme qui se modifie en mémoire (SMC)

Le **Self-Modifying Code** (SMC) est une technique où un programme modifie ses propres instructions pendant l'exécution. C'est utilisé dans les packers, les protections logicielles, et les virus polymorphiques.

Pour que cela fonctionne, la page mémoire contenant le code doit être marquée **writable** (ce qu'elle n'est généralement pas). On peut la rendre writable avec le syscall `mprotect` :

```nasm
; mprotect(adresse, taille, PROT_READ|PROT_WRITE|PROT_EXEC)
; PROT_READ=1, PROT_WRITE=2, PROT_EXEC=4 → flags = 7
mov rax, 10            ; syscall mprotect
mov rdi, page_address  ; adresse de la page (alignée sur 4096)
mov rsi, 4096          ; taille
mov rdx, 7             ; PROT_READ | PROT_WRITE | PROT_EXEC
syscall
```

Après cela, écrire à une adresse dans le segment `.text` modifie les instructions elles-mêmes.

### Injecter du code dans un autre processus

```c
// En C, via ptrace (Linux) :
ptrace(PTRACE_ATTACH, pid, NULL, NULL);   // attacher au processus
ptrace(PTRACE_POKETEXT, pid, addr, data); // écrire en mémoire du processus
ptrace(PTRACE_CONT, pid, NULL, NULL);     // reprendre l'exécution
```

### Hooking de fonctions

Le hooking consiste à **intercepter** un appel de fonction en écrasant les premiers octets de la fonction avec un `jmp` vers notre propre code. Utilisé dans les antivirus (pour intercepter les appels API), les cheats de jeux, et les rootkits.

```nasm
; Patch de la fonction cible pour la rediriger vers notre_hook :
; Les 5 octets d'un JMP relatif 32 bits : [0xE9] [offset sur 4 octets]
; offset = adresse_hook - (adresse_cible + 5)

mov byte [fonction_cible], 0xE9                      ; opcode JMP
mov dword [fonction_cible + 1], notre_hook - (fonction_cible + 5)
```

### Les packers

Un packer est un programme qui compresse/chiffre un exécutable, et au lancement, le "décompresse" en mémoire et y saute. Les malwares l'utilisent pour éviter la détection par signature. Le code original n'est jamais présent tel quel sur le disque.

Structure d'un packer simple :

1. Lire le binaire original depuis `.data` (chiffré/compressé)
2. Allouer une zone mémoire exécutable avec `mmap`
3. Déchiffrer/décompresser le code original dans cette zone
4. Fixer les relocations si nécessaire
5. Sauter au point d'entrée original

### Les virus polymorphiques

Un virus polymorphique chiffre son propre corps avec une clé différente à chaque infection. Seul le petit bout de code de déchiffrement (le "déchiffreur") reste constant, mais lui-même peut être muté. Le but est d'éviter la détection par signatures statiques.

### Les techniques anti-débogage

Les malwares utilisent diverses techniques pour détecter s'ils sont debuggués :

```nasm
; Technique 1 : INT 3 / IsDebuggerPresent
; Un débogueur place des breakpoints via l'instruction 0xCC (INT 3)
; Si le programme voit 0xCC à son propre code, il sait qu'il est debuggué

; Technique 2 : timing
; Sous débogueur, l'exécution est très lente
; On mesure le temps d'une opération et on compare à un seuil

; Technique 3 : ptrace
; Un processus ne peut être tracé que par un seul ptrace à la fois
; Si ptrace(PTRACE_TRACEME) échoue, c'est qu'on est déjà sous débogueur
```

### Les injections de shellcode

Pour injecter du shellcode dans un programme :

1. Trouver un buffer overflow ou une autre vulnérabilité
2. Contourner NX (via ROP) et ASLR (via fuite d'adresse)
3. Écrire le shellcode (null-free si transmis via str fonctions)
4. Déclencher l'exécution

**Shellcode null-free** : évite les octets `0x00` (qui terminent les chaînes en C). Par exemple, au lieu de `mov rax, 59`, on utilise :

```nasm
xor rax, rax
mov al, 59          ; seul l'octet bas est utilisé, pas de null
```

---

## Annexe A : Référence rapide des instructions

```
┌──────────────┬─────────────────────────────────────────────────────────┐
│ Instruction  │ Description                                              │
├──────────────┼─────────────────────────────────────────────────────────┤
│ mov dst, src │ Copie src dans dst                                       │
│ movzx d, s   │ Copie avec zero-extension                               │
│ movsx d, s   │ Copie avec sign-extension                               │
│ lea dst, [x] │ Charge l'adresse effective dans dst (pas d'accès mémoire)│
│ xchg d, s    │ Échange les deux opérandes                              │
├──────────────┼─────────────────────────────────────────────────────────┤
│ add dst, src │ dst = dst + src                                          │
│ adc dst, src │ dst = dst + src + CF                                    │
│ sub dst, src │ dst = dst - src                                          │
│ sbb dst, src │ dst = dst - src - CF                                    │
│ mul src      │ RDX:RAX = RAX * src (non signé)                         │
│ imul ...     │ Multiplication signée (plusieurs formes)                 │
│ div src      │ RAX = RDX:RAX / src, RDX = reste (non signé)           │
│ idiv src     │ Division signée                                          │
│ inc dst      │ dst++                                                    │
│ dec dst      │ dst--                                                    │
│ neg dst      │ dst = -dst                                               │
├──────────────┼─────────────────────────────────────────────────────────┤
│ and dst, src │ AND bit-à-bit                                            │
│ or  dst, src │ OR bit-à-bit                                             │
│ xor dst, src │ XOR bit-à-bit                                            │
│ not dst      │ NOT bit-à-bit (ne modifie pas les flags !)              │
│ test d, s    │ AND bit-à-bit sans modifier les opérandes (flags seuls) │
├──────────────┼─────────────────────────────────────────────────────────┤
│ shl dst, n   │ Décalage logique gauche de n bits                       │
│ shr dst, n   │ Décalage logique droit de n bits                        │
│ sar dst, n   │ Décalage arithmétique droit (préserve le signe)         │
│ rol dst, n   │ Rotation gauche de n bits                               │
│ ror dst, n   │ Rotation droite de n bits                               │
│ rcl dst, n   │ Rotation gauche à travers CF                            │
│ rcr dst, n   │ Rotation droite à travers CF                            │
├──────────────┼─────────────────────────────────────────────────────────┤
│ cmp dst, src │ dst - src → met les flags (ne modifie pas dst)         │
│ jmp label    │ Saut inconditionnel                                      │
│ je/jz label  │ Saut si ZF=1 (égal / zéro)                             │
│ jne/jnz      │ Saut si ZF=0                                            │
│ jl label     │ Saut si moins (signé) : SF≠OF                          │
│ jg label     │ Saut si plus grand (signé) : ZF=0 & SF=OF              │
│ jle label    │ Saut si ≤ (signé) : ZF=1 | SF≠OF                      │
│ jge label    │ Saut si ≥ (signé) : SF=OF                              │
│ jb label     │ Saut si en-dessous (non signé) : CF=1                  │
│ ja label     │ Saut si au-dessus (non signé) : CF=0 & ZF=0            │
│ js label     │ Saut si SF=1 (négatif)                                  │
│ jo label     │ Saut si OF=1 (overflow)                                 │
├──────────────┼─────────────────────────────────────────────────────────┤
│ push src     │ RSP -= 8 ; mem[RSP] = src                               │
│ pop dst      │ dst = mem[RSP] ; RSP += 8                               │
│ call label   │ push RIP_suivant ; jmp label                            │
│ ret          │ pop RIP                                                  │
│ leave        │ mov rsp, rbp ; pop rbp                                  │
├──────────────┼─────────────────────────────────────────────────────────┤
│ syscall      │ Appel système                                            │
│ int 0x80     │ Appel système (ancienne méthode 32 bits, obsolète)      │
│ nop          │ No operation (1 cycle, ne fait rien)                    │
│ hlt          │ Arrête le processeur (Ring 0 uniquement)                │
│ cpuid        │ Retourne les infos sur le processeur                    │
│ rdtsc        │ Lit le compteur de cycles (timestamp counter)           │
└──────────────┴─────────────────────────────────────────────────────────┘
```

---

## Annexe B : Les syscalls les plus courants (Linux x86-64)

```
┌─────┬─────────────┬────────────────────────────────────────────────┐
│ RAX │ Nom         │ Prototype                                       │
├─────┼─────────────┼────────────────────────────────────────────────┤
│  0  │ read        │ ssize_t read(int fd, void *buf, size_t count)  │
│  1  │ write       │ ssize_t write(int fd, void *buf, size_t count) │
│  2  │ open        │ int open(char *path, int flags, mode_t mode)   │
│  3  │ close       │ int close(int fd)                              │
│  4  │ stat        │ int stat(char *path, struct stat *buf)         │
│  5  │ fstat       │ int fstat(int fd, struct stat *buf)            │
│  9  │ mmap        │ void *mmap(addr, len, prot, flags, fd, off)   │
│  10 │ mprotect    │ int mprotect(void *addr, size_t len, int prot) │
│  11 │ munmap      │ int munmap(void *addr, size_t length)          │
│  12 │ brk         │ int brk(void *addr)                            │
│  20 │ getpid      │ pid_t getpid(void)                             │
│  32 │ dup         │ int dup(int oldfd)                             │
│  33 │ dup2        │ int dup2(int oldfd, int newfd)                 │
│  39 │ getpid      │ pid_t getpid(void)                             │
│  41 │ socket      │ int socket(int domain, int type, int protocol) │
│  42 │ connect     │ int connect(int fd, struct sockaddr*, socklen) │
│  43 │ accept      │ int accept(int fd, struct sockaddr*, socklen*) │
│  44 │ sendto      │ ssize_t sendto(...)                            │
│  45 │ recvfrom    │ ssize_t recvfrom(...)                          │
│  49 │ bind        │ int bind(int fd, struct sockaddr*, socklen_t)  │
│  50 │ listen      │ int listen(int fd, int backlog)                │
│  57 │ fork        │ pid_t fork(void)                               │
│  59 │ execve      │ int execve(char *path, char **argv, char **env)│
│  60 │ exit        │ void exit(int status)                          │
│  62 │ kill        │ int kill(pid_t pid, int sig)                   │
│ 231 │ exit_group  │ void exit_group(int status)                    │
└─────┴─────────────┴────────────────────────────────────────────────┘
```

---

## Annexe C : Glossaire

**ABI** : Application Binary Interface — ensemble de conventions définissant comment les programmes interagissent au niveau binaire (conventions d'appel, formats de fichiers, etc.)

**ASLR** : Address Space Layout Randomization — randomisation des adresses de chargement

**BSS** : Block Started by Symbol — segment de données non initialisées

**Canary** : valeur aléatoire placée sur la pile pour détecter les buffer overflows

**CF** : Carry Flag — drapeau de retenue/emprunt

**ELF** : Executable and Linkable Format — format des binaires Linux

**Frame** : zone de la pile allouée pour une invocation de fonction

**Gadget ROP** : séquence d'instructions se terminant par `ret`, utilisée en ROP

**GPR** : General Purpose Register — registre général

**LIFO** : Last In First Out — structure de données pile

**MMU** : Memory Management Unit — unité de gestion mémoire du processeur

**NX/DEP** : Non-Execute / Data Execution Prevention — protection contre l'exécution de données

**OF** : Overflow Flag — drapeau de débordement signé

**PIE** : Position Independent Executable — exécutable indépendant de la position

**RIP** : Instruction Pointer — pointeur d'instruction (adresse de la prochaine instruction)

**ROP** : Return-Oriented Programming — technique d'exploitation chaînant des gadgets

**RSP** : Stack Pointer — pointeur de sommet de pile

**SF** : Sign Flag — drapeau de signe

**Shellcode** : séquence d'octets exécutables injectée lors d'un exploit

**SMC** : Self-Modifying Code — code qui se modifie lui-même

**Syscall** : appel système — interface pour demander des services au noyau

**TLB** : Translation Lookaside Buffer — cache de traduction d'adresses

**ZF** : Zero Flag — drapeau de zéro

---

*Ce cours couvre les fondamentaux et les techniques avancées de l'assembleur x86-64. La maîtrise vient de la pratique : écris des programmes, désassemble des binaires, casse des challenges CTF. Chaque ligne de code machine que tu lis renforce ta compréhension de ce qui se passe réellement dans la machine.*
