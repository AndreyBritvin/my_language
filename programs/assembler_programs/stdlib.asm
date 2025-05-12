section .data

fmt dq "bla\n"

section .text
global out
global hlt
out_2:
    mov rax, 0x01           ; write64 (rdi, rsi, rdx) ... r10, r8, r9
    mov rdi, 1              ; stdout
    mov rsi, fmt
    ; mov rsi, Buffer
    mov rdx, 10             ; strlen (Msg)
    syscall
    ret

hlt:
    mov rax, 0x3C      ; exit64 (rdi)
    xor rdi, rdi
	syscall
    ret
