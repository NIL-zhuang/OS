all:
	nasm -f elf32 -o print.o print.asm
	g++ -g -m32 -std=c++11 main.cpp print.o -o res
