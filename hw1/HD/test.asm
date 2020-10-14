section .data

; 要打印数字就用PrintNumberByChar
    ; ==== strings
    usrMsg : db "Please input x and y :", 0ah, 0h
    lenUsrMsg : equ $ - usrMsg
    run_here_msg :db 'the program run here', 0ah
    lenRunMsg : equ $ - run_here_msg

    ; === store data
    a_num   : times 100 db 0
    middle1 : times 3 db 0
    b_num   : times 100 db 0
    a_len : db 0
    b_len : db 0
    a_sign : db 0
    b_sign : db 0
    ; ==== READ ====
    read_buffer : db 0
    read_len : db 0
    read_sign : db 0
    format_len : db 0
    format_tmp : db 0
    total_size : db 99
    ; ==== PRINT ====
    sign_print : db 0
    len_num_print :db 0
    ; ==== ADD ====
    middle2 : times 5 db 0
    add_res : times 100 db 0
    add_sign : db 0

    middle3 : times 5 db 0
    ; ==== MUL ====
    tmp_mul_res : times 100 dw 0

section .text
global _start

_start:
    mov ebx, esp        ; for debug
    mov edx, lenUsrMsg
    mov ecx, usrMsg
    mov ebx, 1
    mov eax, 4
    int 80h
    call ReadNumberA
    call ReadNumberB
    call FormatNumebrA
    call FormatNumberB
    call ADD_Number
    mov eax, add_res
    call PrintNumberByChar
    call MUL_Number
    call Quit

; ======= ADD =====
ADD_Number:
    push esi
    push edx
    push ecx
    push ebx
    push eax
    ; 处理加法的符号
    mov al, [a_sign]
    mov [add_sign], al
    ; cl存放计数器，eax, ebx, edx分别是几条数字的尾部
    mov cl, 100
    mov eax, a_num
    add eax, 99
    mov ebx, b_num
    add ebx, 99
    mov edx, add_res
    add edx, 99
    ; 用ch作为加法的中间变量
    ADD_Number_Start:
        ADD_Number_Loop:
            cmp cl, 0
            jz ADD_Number_Format
            mov ch, byte[ebx]
            add ch, byte[eax]
            mov [edx], ch
            ; push eax
            ; mov eax, 0
            ; mov al, ch
            ; call IprintLF
            ; pop eax
            dec eax
            dec ebx
            dec cl
            dec edx
            jmp ADD_Number_Loop
    ADD_Number_Format:
        mov cl, 100         ; cl 计数器
        mov ch, 0           ; ch作为加法的进位
        mov edx, 0
        mov esi, add_res
        add esi, 99         ; esi指向add_res的行尾
        ADD_Number_Format_Loop:
            cmp cl, 0
            je ADD_Number_Format_Finish
            mov dl, byte[esi]   ; dl作为加法的暂存
            add dl, ch
            cmp dl, 9
            jng ADD_Number_Format_ADD
                sub dl, 10              ; 要进位
                mov ch, 1
                jmp ADD_Number_Format_Loop_Flag
            ADD_Number_Format_ADD:      ; 不用进位
                mov ch, 0
            ADD_Number_Format_Loop_Flag:
            mov byte[esi], dl
            dec esi
            dec cl
        jmp ADD_Number_Format_Loop
    ADD_Number_Format_Finish:
    pop eax
    pop ebx
    pop ecx
    pop edx
    pop esi
    ret
; ======= MUL =====
MUL_Number:
push edi
push esi
push edx
push ecx
push ebx
push eax
; for (int i = 0; i < a_len; i++)
;     for (int j = 0; j < b_len; j++)
;         mul_res[99 - i - j] += a_num[99 - i] * b_num[99 - j]
    mov ecx, 0
    mov ch, 49  ; 外层计数器
    mov cl, 49  ; 内层计数器
    mov esi, a_num
    add esi, 99
    mov edi, b_num
    add edi, 99
    mov edx, mul_res    ; edx是尾部地址，这里记得减地址要*2，因为是word
    add edx, 99 * 2

    MUL_Number_Out:
        dec ch
        cmp ch, 0
        je MUL_Number_Fin
        MUL_Number_In:
            dec cl
            cmp cl, 0
            je MUL_Number_Out

            ; eax放了乘法的结果
            mov eax, 0
            mov ebx, 0
            push edx
                mov edx, 0
                mov dl, ch
                mov al, byte[esi - edx]
                mov edx, 0
                mov dl, cl
                mov bl, byte[esi - edx]
                imul eax, ebx

                mov edx, 0
                add dl, cl
                add dl, ch
                imul edx, 2
                mov ebx, edx
            pop edx
            mov word[edx - ebx * 2], ex

            mov ebx, 0
            add bl, cl
            add bl, ch
            mul ebx, 2
            mov [edx - 2*ebx], ax
            jmp MUL_Number_In
    MUL_Number_Fin:
pop eax
pop ebx
pop ecx
pop edx
pop esi
pop edi
ret


; ======= READER =====
FormatNumebrA:
    push eax
    mov al, byte[a_len]
    mov [format_len], al
    mov eax, a_num
    call FormatNumber
    pop eax
    ret
FormatNumberB:
    push eax
    mov al, byte[b_len]
    mov [format_len], al
    mov eax, b_num
    call FormatNumber
    pop eax
    ret
FormatNumber:
    ; eax放数字的首地址
    ; format_len放要格式化的数字的长度
    push esi
    push edx
    push ecx
    push ebx
    push eax
    mov ecx, 0
    mov cl, byte[format_len]       ; ecx存放数字的长度
    mov esi, eax                ; esi存放数字的起始地址
    add eax, 99                 ; eax指向数字尾地址
    mov ebx, esi
    add ebx, ecx
    dec ebx                     ; ebx指向数字的当前尾部
    FormatBegin:
        cmp ecx, 0
        jz FormatAddZero
        mov edx,[ebx]
        mov [eax], edx
        dec ebx
        dec eax
        dec ecx
        jmp FormatBegin

    FormatAddZero:
        mov cl, byte[format_len]
        mov eax, esi    ; eax指向数字的起始地址
        mov ch, 0
        FormatAddZeroLoop:
            cmp cl, 0
            jz FormatFinish
            mov [eax], ch
            dec cl
            inc eax
            jmp FormatAddZeroLoop
    FormatFinish:
    pop eax
    pop ebx
    pop ecx
    pop edx
    pop esi
    ret

ReadNumberA:
    push eax
    mov eax, a_num
    call ReadNumber
    mov al, byte[read_sign]
    mov [a_sign], al
    mov al, [read_len]
    mov [a_len], al
    pop eax
    ret

ReadNumberB:
    push eax
    mov eax, b_num
    call ReadNumber
    mov al, byte[read_sign]
    mov [b_sign], al
    mov al, [read_len]
    mov [b_len], al
    pop eax
    ret

ReadNumber:
    ; 把要放入数字的地址放在eax里
    ; 把读入数字的长度放在ecx然后写入read_len里
    push esi
    push edx
    push ecx
    push ebx
    push eax

    ; 初始化read的变量
    mov bl, 0
    mov [read_buffer], bl
    mov [read_len], bl
    mov [read_sign], bl

    mov esi, eax    ; 用esi来存放首地址
    ReadChar:
        ; 把数字读到read_buffer里
        mov eax, 3
        mov ebx, 0
        mov ecx, read_buffer
        mov edx, 1
        int 80h
        mov cl, byte[read_buffer]
        ; 如果读到空格或者回车，就结束这个活动
        cmp cl, ' '
        je FinishReadNumber
        cmp cl, 0ah
        je FinishReadNumber
        ; 处理符号
        cmp cl, '-'
        jne NotNegativeNumber
            ; 遇到负号
            mov eax, 1
            mov [read_sign], eax
            jmp ReadChar
        NotNegativeNumber:
            sub cl, 48
            mov [esi], cl
            inc esi
            inc byte[read_len]
            jmp ReadChar
    FinishReadNumber:
        pop eax
        pop ebx
        pop ecx
        pop edx
        pop esi
        ret

; ======= PRINTER=====
PrintSpace:
    push eax
    mov eax, ' '
    call PrintChar
    pop eax
    ret
PrintNumberByChar:
    ; 将首字母放到eax后调用PrintNumberByChar
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
    cmp bl, 0   ; notice: 这里用的是bl而不是ebx，因为sign_print前一个值是10，合到ebx里正好是1280+sign
    je PrintSignRet
        mov eax, '-'
        call PrintChar
    PrintSignRet:
        pop eax
        ret

PrintNumberList:
    ; 这是我们现在要用的打印数字的函数
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