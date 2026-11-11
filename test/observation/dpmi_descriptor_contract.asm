; Independent INT31 descriptor validation test; immutable original DOSX.
bits 16
org 100h
    mov sp, stack_top
    mov bx, (image_end-$$+100h+15)/16
    mov ah, 4Ah
    int 21h
    jc failed
    mov ax, 1687h
    int 2Fh
    test ax, ax
    jnz failed
    mov [entry], di
    mov [entry+2], es
    mov bx, si
    test bx, bx
    jz .enter
    mov ah, 48h
    int 21h
    jc failed
    mov es, ax
.enter:
%ifdef CLIENT32
    mov ax, 1
%else
    xor ax, ax
%endif
    call far [entry]
    jc failed
    push ds
    pop es
    xor ax, ax
    mov cx, 1
    int 31h
    jc failed
    mov [selector], ax
    mov bx, ax
    mov edi, original
    mov ax, 000Bh
    int 31h
    jc failed
    mov eax, [original]
    mov [invalid], eax
    mov eax, [original+4]
    mov [invalid+4], eax
    and byte [invalid+5], 0EFh ; Reject a system descriptor.
    call reject_set
    mov al, [original+5]
    xor al, 20h              ; Reject a different DPL.
    mov [invalid+5], al
    call reject_set
    mov bx, [selector]
    mov ax, 0001h
    int 31h
    jc failed
    mov bx, [selector]
    call reject_get
    mov bx, 0FFF8h           ; Outside the original descriptor table.
    call reject_get
    mov edx, success
    mov ah, 09h
    int 21h
    mov ax, 4C00h
    int 21h
reject_set:
    mov bx, [selector]
    mov edi, invalid
    mov ax, 000Ch
    int 31h
    jnc failed
    mov bx, [selector]
    mov edi, observed
    mov ax, 000Bh
    int 31h
    jc failed
    mov eax, [original]
    cmp eax, [observed]
    jne failed
    mov eax, [original+4]
    cmp eax, [observed+4]
    jne failed
    ret
reject_get:
    mov dword [observed], 0A5A5A5A5h
    mov dword [observed+4], 0A5A5A5A5h
    mov edi, observed
    mov ax, 000Bh
    int 31h
    jnc failed
    cmp dword [observed], 0A5A5A5A5h
    jne failed
    cmp dword [observed+4], 0A5A5A5A5h
    jne failed
    ret
failed:
    mov edx, failure
    mov ah, 09h
    int 21h
    mov ax, 4C01h
    int 21h
entry dd 0
selector dw 0
original times 8 db 0
invalid times 8 db 0
observed times 8 db 0
success:
%ifdef CLIENT32
    db 'S38_DESCRIPTOR_CLIENT32_OK',13,10
%endif
    db 'S38_DESCRIPTOR_REJECTION_PRESERVATION_OK',13,10,'$'
failure db 'S38_FAIL_DESCRIPTOR_CONTRACT',13,10,'$'
times 2048 db 0
stack_top:
image_end:
