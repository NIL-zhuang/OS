%include 'functions.asm'

section .text
global _start

_start:
    mov eax, 90
    mov ecx, 10
    div ecx
    call PrintCharLF
    mov eax, edx
    call PrintCharLF
    call quit