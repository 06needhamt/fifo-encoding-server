.text

.global spinlock
.global on_error
.global park
.global unpark

.global busy
.global flag

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
movq %rcx, %rax
lodsb
pop %rcx
pop %rax
ret

park:
push %r9
lea busy(%rip), %r9
movq %r9, %rax
cmp $0xFF, %r9
pop %r9
je halt
cmp $0, %r9
jg unpark
movq $0, %rax
movq $0, busy(%rip)
emms
ret

unpark:
movq $1, %rax
pop %rcx
push %rcx
ret

halt:
emms
cli
hlt

.data
busy: .byte 0x00
flag: .byte 0xFF
