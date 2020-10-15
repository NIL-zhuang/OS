section .data

; 要打印数字就用PrintNumberByChar
; 用middle来避免某些奇怪的长度不匹配复写现象，虽然应该是消除了
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
    bigger_ab : db 0        ; 如果a_num大于b_num，就置为0，否则置为1
    middle2 : times 5 db 0
    add_res : times 100 db 0
    add_sign : db 0

    middle5 : times 5 db 0
    sub_res : times 100 db 0
    sub_sign : db 0

    middle3 : times 5 db 0
    ; ==== MUL ====
    tmp_mul_res : times 100 dw 0
    middle4 : times 3 dw 0
    mul_res : times 100 db 0
    mul_sign : db 0

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

    call ADD_Helper

    mov eax, 10
    call PrintChar

    call MUL_Number
    ; 乘法
    mov eax, 0
    mov al, byte[mul_sign]
    mov byte[sign_print], al
    mov eax, mul_res
    call PrintNumberByChar
    ; 打印一个换行
    mov eax, 10
    call PrintChar
    call Quit

; ======= ADD OR SUB=====
ADD_Helper:
    push edi
    push esi
    push edx
    push ecx
    push ebx
    push eax
    mov ah, byte[a_sign]
    mov al, byte[b_sign]
    cmp al, ah
    jne ADD_Helper_Min
    ADD_Helper_Add:
        ; a和b的符号相同
        mov al, byte[a_sign]
        mov byte[sign_print], al
        call ADD_Number
        mov eax, add_res
        call PrintNumberByChar
        jmp ADD_Helper_Fin
    ADD_Helper_Min:
        ; a和b的符号不同，那就把绝对值更大的数作为减数，另一个做被减数
        call Cmp_Greater
        mov eax, 0
        mov al, byte[bigger_ab]
        cmp al, 0
        jne ADD_Helper_Min_Rev
        ; a_num>=b_num
            mov eax, a_num
            mov ebx, b_num
            mov cl, byte[a_sign]
            jmp ADD_Helper_Min_Do
        ADD_Helper_Min_Rev:
        ; a_num < b_num
            mov eax, b_num
            mov ebx, a_num
            mov cl, byte[b_sign]
        ADD_Helper_Min_Do:
            mov byte[sign_print], cl
            call Sub_Number
            mov eax, sub_res
            call PrintNumberByChar
    ADD_Helper_Fin:
    pop eax
    pop ebx
    pop ecx
    pop edx
    pop esi
    pop edi
    ret

Cmp_Greater:
    ; FIXME: 无法判定到a比b大
    ; 比较a和b，如果a>=b，那就把bigger_ab设为0
    ; 默认是0，所以只要遇到a<b，就把他设成1，然后结束
    push edi
    push esi
    push edx
    push ecx
    push ebx
    push eax
    mov esi, a_num  ; esi指向a头部
    mov edi, b_num  ; edi指向b头部
    mov cl, 0     ; 计数器
    ; bigger_ab = 0
    ; while(++cl!=101)
    ;   if(a[cl]>=b[cl]) continue
    ;   bigger_ab=1;
    Cmp_Greater_Loop:
        inc esi
        inc edi
        inc cl
        cmp cl, 100
        jg Cmp_Greater_Fin
        mov eax, 0
        mov edx, 0
        mov al, byte[esi]   ; ah = a[cl]
        mov dl, byte[edi]   ; al = b[cl]
        cmp eax, edx          ; if(ah > al)
        je Cmp_Greater_Loop
        jg Cmp_Greater_A
        jl Cmp_Greater_B
        Cmp_Greater_A:
            mov ebx, 0
            mov byte[bigger_ab], bl
            jmp Cmp_Greater_Fin
        Cmp_Greater_B:
            mov ebx, 1
            mov byte[bigger_ab], bl
    Cmp_Greater_Fin:
    pop eax
    pop ebx
    pop ecx
    pop edx
    pop esi
    pop edi
    ret
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

Sub_Number:
    ; 将eax存放被减数的地址，ebx存放减数的地址
    push edi
    push esi
    push edx
    push ecx
    push ebx
    push eax
    mov esi, eax
    mov edi, ebx
    mov edx, sub_res
    add esi, 99
    add edi, 99     ; esi被减数尾地址，edi减数尾地址
    add edx, 99     ; edx指向减法结果的尾地址
    mov cl, 100     ; cl 计数器
    mov ch, 0       ; ch 作为借位的carry位。借位不是对被减数-1，而是对减数+1，规避连续0的影响
    Sub_Number_Loop:
        cmp cl, 0
        je Sub_Number_Fin
        mov ah, byte[esi]
        mov al, byte[edi]
        add al, ch          ; ah存被减数，al存减数
        ; al = al + carry
        ; if(ah>=al) ch = 0, ah = ah - al
        ;       else ch = 1, ah = ah+10 - al
        cmp ah, al
        jl Sub_Number_Greater
            ; ah >= al 不用借位
            mov ch, 0
            sub ah, al
        jmp Sub_Number_Write
        Sub_Number_Greater:
            ; ah < al 要借位
            mov ch, 1
            add ah, 10
            sub ah, al
        Sub_Number_Write:
            mov byte[edx], ah
        dec edx
        dec cl
        dec esi
        dec edi
        jmp Sub_Number_Loop
    Sub_Number_Fin:
    pop eax
    pop ebx
    pop ecx
    pop edx
    pop esi
    pop edi
    ret
; ======= MUL =====
MUL_Number:
    push edi
    push esi
    push edx
    push ecx
    push ebx
    push eax

    ; 处理符号
    mov al, byte[a_sign]
    mov ah, byte[b_sign]
    cmp al, ah
    je MUL_Number_Start
    mov al, 1
    mov byte[mul_sign], al

    MUL_Number_Start:
    ; for (int i = 0; i < a_len; i++, a--)
    ;     for (int j = 0; j < b_len; j++, b--)
    ;         mul_res[99 - i - j] += *a * *b
        mov ecx, 0
        ; mov ch, 0  外层计数器
        ; mov cl, 0  内层计数器

        mov esi, a_num  ; 外部数，esi指向a的尾部
        add esi, 99

        MUL_Number_Out:
            mov edi, b_num
            add edi, 99     ; 内部数，edi指向b的尾部
            mov cl, 0
            MUL_Number_In:
                ; tmp_val = a[99-i] * b[99-j]
                mov eax, 0
                mov al, byte[esi]
                mov ebx, 0
                mov bl, byte[edi]
                imul eax, ebx                    ; 此时eax存放了tmp_val值
                push esi
                push edi
                    mov esi, eax                    ; 将乘积放在了esi里
                    ; t = (99 -i -j) * 2
                    mov eax, 0
                    mov al, cl
                    mov ebx, 0
                    mov bl, ch
                    add eax, ebx
                    mov edi, 99
                    sub edi, eax
                    imul edi, 2                  ; 此时edi放的是 (99 - i - j) * 2, 地址要*2因为是word
                    ; res[t/2] += tmp_val
                    mov edx, tmp_mul_res        ; tmp_mul_res的起始地址
                    add edx, edi                ; 此时edx指向的是要加值的地方
                    mov eax, 0
                    mov ax, word[edx]
                    add eax, esi
                    mov word[edx], ax

                pop edi
                pop esi
            MUL_Number_In_Fin:
                inc cl
                dec edi
                cmp cl, 49
                jne MUL_Number_In
        MUL_Number_Out_Fin:
            inc ch
            dec esi
            cmp ch, 49
            jne MUL_Number_Out

        MUL_Number_Add:
            ; 暂时乘积存放在tmp_mul_res以字形势存放，现在要把他放到mul_res里
            mov esi, tmp_mul_res
            add esi, 99*2       ; esi指向tmp_res的根地址
            mov ecx, 100        ; ecx 计数
            mov edi, mul_res
            add edi, 99         ; edi指向mul_res的根地址
            mov ebx, 0          ; ebx作为进位的carry
            MUL_Number_Add_Loop:
                cmp ecx, 0
                je MUL_Number_Fin
                    mov eax, 0
                    mov edx, 0
                    mov ax, word[esi]
                    add eax, ebx        ; eax = tmp_res[i] + carry
                    mov ebx, 10
                    div ebx             ; 进位放在eax里，余数放在edx里
                    mov ebx, eax        ; eax是商，放在ebx里
                    mov byte[edi], dl   ; dl放的是余数
                dec ecx
                dec edi
                sub esi, 2
                jmp MUL_Number_Add_Loop
    MUL_Number_Fin:
    pop eax
    pop ebx
    pop ecx
    pop edx
    pop esi
    pop edi
    ret

PrintTmpMulNumber:
    push edx
    push ecx
    push ebx
    push eax
        mov ebx, tmp_mul_res
        mov ecx, 100
        PrintTmpMulNumberLoop:
            cmp ecx, 0
            je PrintTmpMulNumberFin
            mov eax, 0
            mov ax, word[ebx]
            call Iprint
            call PrintSpace
            dec ecx
            add ebx, 2
            jmp PrintTmpMulNumberLoop
    PrintTmpMulNumberFin:
    pop eax
    pop ebx
    pop ecx
    pop edx
    ret


; ======= READER =====
FormatNumebrA:
    push eax
    mov al, byte[a_len]
    mov byte[format_len], al
    mov eax, a_num
    call FormatNumber
    pop eax
    ret
FormatNumberB:
    push eax
    mov al, byte[b_len]
    mov byte[format_len], al
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
        mov dl,byte[ebx]
        mov byte[eax], dl
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
            mov byte[eax], ch
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
    mov byte[a_sign], al
    mov al, byte[read_len]
    mov byte[a_len], al
    pop eax
    ret

ReadNumberB:
    push eax
    mov eax, b_num
    call ReadNumber
    mov al, byte[read_sign]
    mov byte[b_sign], al
    mov al, byte[read_len]
    mov byte[b_len], al
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
    mov byte[read_buffer], bl
    mov byte[read_len], bl
    mov byte[read_sign], bl

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
            mov byte[read_sign], al
            jmp ReadChar
        NotNegativeNumber:
            sub cl, 48
            mov byte[esi], cl
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
    push edx
    push eax
    call GetNumberLength
    mov edx, 0
    mov dl, byte[len_num_print]
    cmp dl, 0
    jnz PrintNumberByCharNotZero
        mov eax, 0
        call PrintNumber
        jmp PrintNumberByCharFin
    PrintNumberByCharNotZero:
        call PrintSign
        call PrintNumberList
    PrintNumberByCharFin:
        pop eax
        pop edx
        ret

PrintSign:
    push eax
    push ebx
    mov bl, byte[sign_print]
    ; mov eax, ebx
    ; call IprintLF
    cmp bl, 0   ; notice: 这里用的是bl而不是ebx，因为sign_print前一个值是10，合到ebx里正好是1280+sign
    je PrintSignRet
        mov eax, '-'
        call PrintChar
    PrintSignRet:
        pop ebx
        pop eax
        ret

PrintNumberList:
    ; 这是我们现在要用的打印数字的函数
    ; 用于printNumberByChar里，打印完符号位之后打印整个数字
    ; 其中eax存放的还是首地址
    push edx
    push ecx
    push ebx
    push eax
    mov ebx, eax    ; ebx放数字首地址
    mov ecx, 0
    mov cl, 100
    mov edx, 0
    mov dl, byte[len_num_print]
    sub cl, dl    ; ecx放要打印数字的起始地址
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
        pop edx
        ret


GetNumberLength:
    ; 提前将要取长度的数的地址放在 eax 寄存器里
    ; 将长度放在len_num_print地址里
    push ecx
    push ebx
    push eax
    push edx
    mov ecx, 0
    mov cl, 100
    mov ebx, eax    ; ebx是数字的首地址
    GetLengthLoop:
        mov edx, 0
        mov dl, byte[ebx]
        cmp dl, 0 ;如果当前位置不是0，那么就获得了长度
        jnz finishGetLen
        inc ebx             ; len--, *p++
        dec cl
        cmp cl, 0    ; 如果长度变为0，那也结束运行
        jz finishGetLen
        jmp GetLengthLoop
    finishGetLen:
    mov byte[len_num_print], cl
    pop edx
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