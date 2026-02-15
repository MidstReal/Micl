global _start

section .data

x db "Hello, World!", 10, ""
x_len equ $ - x

section .text
_start:

mov rsi, x
mov rax, 1
mov rdi, 1
mov rdx, x_len
syscall

mov rax, 60
syscall

