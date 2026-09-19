; Independent test of original DOSX INT31/0B debug services. The dispatch
; table is authoritative; internal function comments still call them 0Axx.
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
    mov ax, 0202h
    mov bl, 1
    int 31h
    jc failed
    mov [old_handler], dx
    mov [old_handler+2], cx
    mov ax, 0203h
    mov bl, 1
    mov cx, cs
    mov dx, debug_handler
    int 31h
    jc failed
    mov byte [stage], 'B'
    mov ax, 0006h
    mov bx, ds
    int 31h
    jc failed
    shl ecx, 16
    mov cx, dx
    add ecx, watched
    mov ebx, ecx
    shr ebx, 16
    mov byte [stage], 'W'
    mov dx, 0101h             ; One byte, write watchpoint.
    mov ax, 0B00h
    int 31h
    jc failed
    mov [watch_handle], bx
    mov byte [stage], '2'
    mov byte [watched], 7
    cmp word [hits], 1
    jne failed
    mov dx, hit_ok
    mov ah, 09h
    int 21h
    mov byte [stage], '3'
    mov bx, [watch_handle]
    mov ax, 0B02h
    int 31h
    jc failed
    test ax, 1
    jz failed
    mov byte [stage], '4'
    mov ax, 0B03h
    mov bx, [watch_handle]
    int 31h
    jc failed
    mov ax, 0B02h
    mov bx, [watch_handle]
    int 31h
    jc failed
    test ax, 1
    jnz failed
    mov ax, 0B01h
    mov bx, [watch_handle]
    int 31h
    jc failed
    mov byte [watched], 8
    cmp word [hits], 1
    jne failed
    mov ax, 0B01h
    mov bx, [watch_handle]
    int 31h
    jnc failed
    mov ax, 0203h
    mov bl, 1
    mov dx, [old_handler]
    mov cx, [old_handler+2]
    int 31h
    jc failed
    mov dx, success
    mov ah, 09h
    int 21h
    mov ax, 4C00h
    int 21h
debug_handler:
    inc word [hits]
    retf
failed:
    mov dx, failure
    mov ah, 09h
    int 21h
    mov ax, 4C01h
    int 21h
entry dd 0
old_handler dd 0
watch_handle dw 0
hits dw 0
watched db 0
hit_ok db 'S38_DEBUG_WRITE_HIT_OK',13,10,'$'
success db 'S38_DEBUG_LIFECYCLE_OK',13,10,'$'
failure db 'S38_FAIL_STAGE='
stage db '0',13,10,'$'
times 4096 db 0
stack_top:
image_end:
