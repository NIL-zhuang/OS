StringLength:
; the start location of the string is stored in eax
; finally, eax stores the length of string
    push ebx
    mov ebx, eax
nextchar:
    cmp byte[eax], 0
    jz finished
    inc eax
    jmp nextchar
finished:
    sub eax, ebx
    pop ebx
    ret

; ==============
; void sprint string message
Sprint:
    push edx
    push ecx
    push ebx
    push eax
    call StringLength
    mov edx, eax
    pop eax
    mov ecx, eax
    mov ebx, 1
    mov eax, 4
    int 80h
    pop ebx
    pop ecx
    pop edx
    ret

; =====
; 用eax传参
PrintCharLF:
    push edx
    push ecx
    push ebx
    push eax
    add eax, 48
    push eax
    mov ecx, esp
    mov eax, 4
    mov ebx, 1
    mov edx, 1
    int 80h
    pop eax
    mov eax, 10
    push eax
    mov eax, 4
    int 80h
    pop eax
    pop eax
    pop ebx
    pop ecx
    pop edx
    ret

SprintLF:
    call Sprint
    push eax
    mov eax, 0ah
    push eax
    mov eax, esp
    call Sprint
    pop eax
    pop eax
    ret

;========
; void exit()
; exit program and restore resources
quit:
    mov ebx, 0
    mov eax, 1
    int 80h
    ret

; =====
; void iprint integer number
; integer printing function(itoa)
Iprint:
    push eax
    push ecx
    push edx
    push esi
    mov ecx, 0  ; count of how many bytes we need to print in the end
divideLoop:
    inc ecx
    mov edx, 0
    mov esi, 10
    idiv esi
    add edx, 48
    push edx
    cmp eax, 0
    jnz divideLoop
printLoop:
    dec ecx
    mov eax, esp
    call Sprint
    pop eax
    cmp ecx, 0
    jnz printLoop

    pop esi
    pop edx
    pop ecx
    pop eax
    ret

IprintLF:
    call Iprint
    push eax
    mov eax, 0ah
    push eax
    mov eax, esp
    call Sprint
    pop eax
    pop eax
    ret