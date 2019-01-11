.text

.global spinlock

spinlock:
xor %rax, %rax
nop
lea flag(%rip), %rax
cmp $0, %rax
jne spinlock
ret

flag: .byte 0xFF
