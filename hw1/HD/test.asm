section .data

a_num         : times 100 db 0
run_here_msg :db 'the program run here', 0ah
lenRunMsg : equ $ - run_here_msg

; ==== PRINT ====
sign_print : db 0
len_num_print :db 0

section .text
global _start

_start:
    mov ebx, esp        ; for debug
    mov eax, a_num
    mov ebx, 3
    mov [eax+95], ebx
    mov ebx, 5
    mov [eax+98], ebx
    ; mov ebx, 0
    ; mov [sign_print], ebx
    call PrintNumberByChar
    call Quit



PrintNumberByChar:
    ; 将数字的首地址存放在eax寄存器中
    ; 打印一个数字
    call GetNumberLength
    mov edx, [len_num_print]
    cmp edx, 0
    jnz PrintNumberByCharNotZero
    mov eax, 0
    call PrintNumber
    ret
    PrintNumberByCharNotZero:
        call PrintSign
        call PrintNumberList
        ret

PrintSign:
    push eax
    mov ebx, [sign_print]
    ; mov eax, ebx
    ; call IprintLF
    ; FIXME: 不管怎么搞这里都会打印负号
    cmp ebx, 0
    je PrintSignRet
        mov eax, '-'
        call PrintChar
    PrintSignRet:
        pop eax
        ret

PrintNumberList:
    ; 用于printNumberByChar里，打印完符号位之后打印整个数字
    ; 其中eax存放的还是首地址
    push ecx
    push ebx
    push eax
    mov ebx, eax    ; ebx放数字首地址
    mov ecx, 100
    sub ecx, [len_num_print]    ; ecx放要打印数字的起始地址
    PrintNumberListLoop:
        cmp ecx, 100
        je PrintNumberListRet
        mov eax, [ebx+ecx]
        call PrintNumber
        inc ecx
        jmp PrintNumberListLoop
    PrintNumberListRet:
        pop eax
        pop ebx
        pop ecx
        ret


GetNumberLength:
    ; 提前将要取长度的数的地址放在 eax 寄存器里
    ; 将长度放在len_num_print地址里
    push ecx
    push ebx
    push eax
    mov ecx, 100
    mov ebx, eax    ; ebx是数字的首地址
    GetLengthLoop:
        cmp byte[ebx], 0 ;如果当前位置不是0，那么就获得了长度
        jnz finishGetLen
        inc ebx             ; len--, *p++
        dec ecx
        cmp ecx, 0    ; 如果长度变为0，那也结束运行
        jz finishGetLen
    jne GetLengthLoop
    finishGetLen:
    mov [len_num_print], ecx
    pop eax
    pop ebx
    pop ecx
    ret

PrintNumber:
    push eax
    add eax, 48
    call PrintChar
    pop eax
    ret

PrintChar:
    ; 将字符放在eax里，打印出eax的字符
    push edx
    push ecx
    push ebx
    push eax
    mov ecx, esp
    mov eax, 4
    mov ebx, 1
    mov edx, 1
    int 80h
    pop eax
    pop ebx
    pop ecx
    pop edx
    ret


PrintNumberLF:
    ; 往eax里放对应的数字，可以以一个换行打印出来
    call PrintNumber
    push eax
    mov eax, 0ah
    call PrintChar
    pop eax
    ret


Iprint:
    ; =====
    ; void iprint integer number,将放在eax里的数字打印出来
    ; integer printing function(itoa)
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


Sprint:
    ; ==============
    ; void sprint string message
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

Quit:
    mov ebx, 0
    mov eax, 1
    int 80h
    ret
RunHere:
    push edx
    push ecx
    push ebx
    push eax
    mov edx, lenRunMsg
    mov ecx, run_here_msg
    mov ebx, 1
    mov eax, 4
    int 80h
    pop eax
    pop ebx
    pop ecx
    pop edx
    ret

tail: