./compiler
nasm -f elf64 temp.asm -o temp.o
ld temp.o -o program
