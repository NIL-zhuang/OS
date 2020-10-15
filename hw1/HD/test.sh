nasm -f elf -F dwarf -g test.asm
ld -m elf_i386 test.o -o runnable
rm test.o
./runnable
rm runnable