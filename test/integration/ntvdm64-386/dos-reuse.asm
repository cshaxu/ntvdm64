; A new DOS VM after both identity witnesses have actually retired.
; Require real VMM handle reuse, reject its previous generation and retain
; a valid counted completion for the new generation. Test only.
bits 16
org 0x100
push cs
pop ds
xor ax, ax
mov es, ax
xor di, di
mov ax, 0x1684
mov bx, 0x7f80
int 0x2f
mov [api], di
mov [api+2], es
mov ax, es
or ax, di
jz write_result
xor ax, ax
call far [api]
jc write_result
cmp eax, 0x4e544d56
jne write_result
mov [vm], ebx
mov [generation], edx
mov [generation+4], esi
mov dx, previous_a
call read_previous
jnc matched
mov dx, previous_b
call read_previous
jc write_result
matched:
mov eax, [input+12]
mov edx, [input+16]
mov [old_generation], eax
mov [old_generation+4], edx
cmp eax, [generation]
jne different
cmp edx, [generation+4]
je write_result
different:
mov ebx, [vm]
mov edx, [generation]
mov esi, [generation+4]
mov ax, 3
call far [api]
jc write_result
; The old identity must not touch the new semaphore or host wake queue.
mov edx, [old_generation]
mov esi, [old_generation+4]
mov ax, 3
call far [api]
jnc write_result
inc dword [rejected]
mov ax, 8
call far [api]
jnc write_result
inc dword [rejected]
mov ax, 2
call far [api]
jc write_result
inc dword [consumed]
mov dword [error], 0
write_result:
mov dx, output_name
xor cx, cx
mov ah, 0x3c
int 0x21
jc failed
mov bx, ax
mov dx, report
mov cx, report_end-report
mov ah, 0x40
int 0x21
mov ah, 0x3e
int 0x21
mov ax, 0x4c00
int 0x21
failed:
mov ax, 0x4c01
int 0x21
read_previous:
mov ax, 0x3d00
int 0x21
jc .fail
mov bx, ax
mov dx, input
mov cx, 72
mov ah, 0x3f
int 0x21
pushf
push ax
mov ah, 0x3e
int 0x21
pop ax
popf
jc .fail
cmp ax, 72
jne .fail
cmp dword [input], 'VMID'
jne .fail
mov eax, [input+8]
cmp eax, [vm]
jne .fail
clc
ret
.fail:
stc
ret
api: dd 0
report: db 'VRUS'
vm: dd 0
generation: dq 0
old_generation: dq 0
error: dd 1
rejected: dd 0
consumed: dd 0
report_end:
previous_a: db 'A.BIN',0
previous_b: db 'B.BIN',0
output_name: db 'REUSE.BIN',0
input: times 72 db 0
