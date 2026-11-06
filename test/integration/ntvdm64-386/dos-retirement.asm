; Independent forced-retirement witness, used only on a copied installation.
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
jz failed
xor ax, ax
call far [api]
jc failed
mov [vm], ebx
mov [generation], edx
mov [generation+4], esi
cmp byte [role], 'A'
jne peer
; Commit identity/report before blocking; returning from the wait is failure.
call write_result
mov dx, pair_ready
xor cx, cx
mov ah, 0x3c
int 0x21
jc failed
mov bx, ax
mov dx, vm
mov cx, 12
mov ah, 0x40
int 0x21
jc failed
cmp ax, 12
jne failed
mov ah, 0x3e
int 0x21
jc failed
mov ax, 2
call far [api]
jmp failed
peer:
mov dx, pair_ready
mov ax, 0x3d00
int 0x21
jnc opened
mov ax, 0x1680
int 0x2f
jmp peer
opened:
mov bx, ax
mov dx, target
mov cx, 12
mov ah, 0x3f
int 0x21
jc failed
cmp ax, 12
jne failed
mov ah, 0x3e
int 0x21
jc failed
xor di, di
xor ax, ax
mov es, ax
mov ax, 0x1684
mov bx, 0x7f81
int 0x2f
mov [killer], di
mov [killer+2], es
mov ax, es
or ax, di
jz failed
kill:
mov ebx, [target]
call far [killer]
jnc killed
cmp eax, 2                       ; valid target has not entered its wait yet
jne failed
mov ax, 0x1680
int 0x2f
jmp kill
killed:
cmp ecx, 0x400
jne failed
mov [blocked], ecx
; A late wake for the original identity must be rejected, not revive A.
mov ebx, [target]
mov edx, [target+4]
mov esi, [target+8]
mov ax, 8
call far [api]
jnc failed
mov dword [rejected], 1
call write_result
mov ax, 0x4c00
int 0x21
failed:
mov dword [error], 1
call write_result
mov ax, 0x4c01
int 0x21
write_result:
mov dx, result_path
xor cx, cx
mov ah, 0x3c
int 0x21
jc .done
mov bx, ax
mov dx, result
mov cx, result_end-result
mov ah, 0x40
int 0x21
mov ah, 0x3e
int 0x21
.done:
ret
api: dd 0
killer: dd 0
target: times 12 db 0
result:
db 'VRET'
vm: dd 0
generation: dq 0
error: dd 0
blocked: dd 0
rejected: dd 0
result_end:
result_path: db '@VM_IDENTITY_RESULT@',0
times 128-($-result_path) db 0
pair_ready: db '@VM_PAIR_READY@',0
times 128-($-pair_ready) db 0
role: db '@VM_ROLE@',0
