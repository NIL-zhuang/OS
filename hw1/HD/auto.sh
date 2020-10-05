nasm -f elf hw.asm
ld -m elf_i386 hw.o -o hw
rm hw.o
./hw
rm hw
