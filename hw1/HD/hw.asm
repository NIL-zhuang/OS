; ----High Precision Digital Program---
; programmed by NIL zhuang  181830266@smail.nju.edu.cn
; using nasm language
; 本代码主要采用约定地址传参的方式进行函数调用

section .data
    userMsg       : db "This is zzy's Program" ,0Ah, "Please input x and y : ", 0Ah, 0h    ;LF, NUL
    lenUserMsg    : equ $ - userMsg
    input         : times 200 db 0                                                      ; a const of userMsg length
    a_len         : dd 0
    b_len         : dd 0
    a_num         : times 100 db 0
    b_num         : times 100 db 0
    add_res       : times 100 db 0
    mul_res       : times 100 db 0
    carry         : db 0  ; carrier
    a_neg         : db 0
    b_neg         : db 0
    buffer:  db 0
    add_neg: db 0
    mul_neg: db 0

    ;what will be used in print numbers
    len_num_print : db 0
    sign_num_print : db 0
    number_print : dd 0
    all_zero_print : db 0

    ; add
    add_temp : db 0
    carry : carry 0
    ; sub
    sub_neg : db 0
    sub_res : times 100 db 0
    sub_tmp : db 0
    ; temp values
    not_zero : db 0

section .text
    global        _start
_start:
; ==== print hello message and need input ====
    mov edx, lenUserMsg
    mov ecx, userMsg
    mov ebx, 1          ; stdout, 文件描述符
    mov eax, 4          ; 系统调用号 sys_write
    int 80h             ; 进入内核态调用内核功能

; =====read number a and b========
; numbers are divided by a space in between
ReadNums:

ReadA:
    mov eax, 3
    mov ebx, 0
    mov ecx, buffer
    mov edx, 1
    int 80h
    mov cl, byte[buffer]

    ; if encounter space, finish reading a_num
    cmp cl, ' '
    je ReadB
    ; if encounter '-', number a is negative
    cmp cl, '-'
    jne NotNegativeA
    mov eax, 1
    mov [a_neg], eax
    jmp ReadA

    NotNegativeA:
    sub cl, 48
    mov edx, [a_len]
    mov [a_num + edx], cl   ; write buffer into number a

    inc byte[a_len]         ; length of a ++
    jmp ReadA

ReadB:
    mov eax, 3
    mov ebx, 0
    mov ecx, buffer
    mov edx, 1
    int 80h
    mov cl, byte[buffer]

    ; if encounter enter, finish reading numbers
    cmp cl, 0Ah
    je FormatNumbers
    cmp cl, '-'
    ; if encounter '-', b is negative
    jne NotNegativeB
    mov eax, 1
    mov [b_neg], eax
    jmp ReadB

    NotNegativeB:
    sub cl, 48
    mov edx, [b_len]
    mov [b_num+edx],cl
    inc byte[b_len]
    jmp ReadB


FormatNumbers:
; make origin forms like 12345xxxxx into 0000012345
; eax stores the total length of spaces, here is 100
; ebx stores the number's length
    HandleA:
    xor eax, eax
    xor ebx, ebx
    mov al, 99  ;notice that a+99 is the 100th num
    mov bl, [a_len]

    SwapA:
        mov cl, [a_num+ebx]
        mov [a_num+eax], cl
        dec eax
        dec ebx
        cmp ebx, 0
        jnl SwapA
        jmp HandleB

    HandleB:
    xor eax, eax
    xor ebx, ebx
    mov al, 99
    mov bl, [b_len]

    SwapB:
        mov cl, [b_num+ebx]
        mov [a_num+eax], cl
        dec eax
        dec ebx
        cmp ebx, 0
        jnl SwapB
        jmp Addition

; ==== add number a and number b ====
Addition:
    call PrintNumberA
    call PrintNumberB



AddNums:

MulNums:

; ==== PRINT ====
PrintChar:
    mov eax, 4
    mov ebx, 1
    mov edx, 1
    int 80h
    ret

PrintSign:
; 提前将符号位存入sign_num, 如果是1的话，打印一个'-'号
    mov eax, [sign_num_print]
    cmp eax, 1
    jnz PrintSignReturn
    mov eax, 4
    mov ebx, 1
    mov ecx, '-'
    mov edx, 1
    int 80h
    PrintSignReturn:
        ret

PrintNumberA:
    ; 将数字的地址放到number_print里，然后打印函数
    push eax
    mov eax, [a_neg]
    mov [sign_num_print], eax
    call PrintSign
    mov eax, [a_num]
    mov [number_print], eax
    mov eax, [a_len]
    mov [len_num_print], eax
    call PrintNumberByChar
    pop eax
    ret

PrintNumberB:
    push eax
    mov eax, [b_neg]
    mov [sign_num_print], eax
    call PrintSign
    mov eax, [b_num]
    mov [number_print], eax
    mov eax, [b_len]
    mov [len_num_print], eax
    call PrintNumberByChar
    pop eax
    ret

PrintNumberByChar:
; ====此时要打印的数的长度存在len_num_print里了
    cmp [len_num_print], 0
    jne PrintNotZero
    mov ecx, '0'
    call PrintChar
    ret
    PrintNotZero:
    ; 要打印的数字不是0
    mov ecx, [len_num_print]
    sub ecx, 1
    mov byte[cl], [number_print+ecx]
    mov ecx, cl
    call PrintChar
    dec len_num_print
    cmp len_num_print, 0
    jne PrintNotZero
    ret

; ====== I/O
PrintNumberByChar:
; 提前将要打印的数的地址放在eax寄存器里
    call GetNumberLength    ; len_num_print
    ret

GetNumberLength:
; 提前将要取长度的数的地址放在 eax 寄存器里
; 将长度放在len_num_print地址里
    push ecx
    push ebx
    mov ecx, 100
    mov ebx, eax
    GetLengthLoop:
        cmp byte[ebx], 0    ;如果当前位置不是0，那么就获得了长度
        jnz finishGetLen
        inc ebx             ; len--, *p++
        dec ecx
        cmp byte[ecx], 0    ; 如果长度变为0，那也结束运行
        jz finishGetLen
    jne GetLengthLoop
    finishGetLen:
    mov [len_num_print], ecx
    pop ebx
    pop ecx
    ret

PrintNumber:
    ; 将数字的地址放在eax里，打印出eax的字符
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
    pop eax
    pop ebx
    pop ecx
    pop edx

;退出程序
Quit:
    mov ebx, 0
    mov eax, 1
    int 80h
    ret