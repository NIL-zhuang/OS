nasm -f elf hello.asm
ld -m elf_i386 hello.o -o runnable
rm hello.o
./runnable
rm runnable
