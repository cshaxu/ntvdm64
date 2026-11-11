; Independent test of original DOSX INT2F/1684 -> VCD -> BOP 53:12.
; No guest component is modified. vxd.c exposes version and port array only.
bits 16
org 100h
    mov sp, stack_top
    mov bx, (image_end - $$ + 100h + 15) / 16
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
    xor ax, ax
    call far [entry]
    jc failed
    mov byte [stage], '1'
    mov ax, 1684h
    mov bx, 0Eh
    xor di, di
    int 2Fh
    mov ax, es
    test ax, ax
    jz failed
    mov [service], di
    mov [service+2], ax
    xor dx, dx
    call far [service]
    cmp ax, 030Ah
    jne failed
    mov byte [stage], '2'
    mov dx, 1
    call far [service]
    test ax, 0FE00h             ; Original WOW port array has only 9 bits.
    jnz failed
    mov byte [stage], '3'
    mov dx, 0FFFFh
    clc
    call far [service]
    jnc failed
    mov dx, success
    mov ah, 09h
    int 21h
    mov ax, 4C00h
    int 21h
failed:
    mov dx, failure
    mov ah, 09h
    int 21h
    mov ax, 4C01h
    int 21h
entry dd 0
service dd 0
success db 'S38_VCD_VERSION_PORTS_REFUSAL_OK',13,10,'$'
failure db 'S38_FAIL_STAGE='
stage db '0',13,10,'$'
times 4096 db 0
stack_top:
image_end:
