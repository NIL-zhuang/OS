
; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;                               syscall.asm
; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;                                                     Forrest Yu, 2005
; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

%include "sconst.inc"

INT_VECTOR_SYS_CALL equ 0x90
_NR_get_ticks       equ 0
_NR_write	    equ 1
_NR_sleep	    equ 2
_NR_my_display_str  equ 3
_NR_P_process       equ 4
_NR_V_process       equ 5


; 导出符号
global	get_ticks
global	write

global sleep
global my_display_str
global P_process
global V_process

bits 32
[section .text]

; ====================================================================
;                              get_ticks
; ====================================================================
get_ticks:
	mov	eax, _NR_get_ticks
	int	INT_VECTOR_SYS_CALL
	ret

; ====================================================================================
;                          void write(char* buf, int len);
; ====================================================================================
write:
        mov     eax, _NR_write
        mov     ebx, [esp + 4]
        mov     ecx, [esp + 8]
        int     INT_VECTOR_SYS_CALL
        ret

sleep:
        mov eax, _NR_sleep
        mov ebx, [esp + 4]
        int INT_VECTOR_SYS_CALL
        ret

my_display_str:
        mov eax, _NR_my_display_str
        mov ebx, [esp + 4]
        mov ecx, [esp + 8]
        int INT_VECTOR_SYS_CALL
        ret

P_process:
        mov eax, _NR_P_process
        mov ebx, [esp + 4]
        int INT_VECTOR_SYS_CALL
        ret

V_process:
        mov eax, _NR_V_process
        mov ebx, [esp + 4]
        int INT_VECTOR_SYS_CALL
        ret
