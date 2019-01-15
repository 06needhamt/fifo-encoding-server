.text

.global spinlock
.global on_error

spinlock:
xor %rax, %rax
nop
lea flag(%rip), %rax
cmp $0, %rax
jne spinlock
ret

on_error:
push %rax
push %rcx
mov %rcx, %rax
lodsb
pop %rcx
pop %rax
ret;

flag: .byte 0xFF
