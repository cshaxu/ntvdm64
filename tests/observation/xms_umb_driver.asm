; Test-only character driver: runs after HIMEM but before DOS ConfigDone
; claims the remaining UMBs. It allocates, releases and rejects double free
; through HIMEM's public entry; it neither supplies nor patches XMS behavior.
bits 16
org 0
    dd 0FFFFFFFFh
    dw 8000h
    dw strategy, interrupt
    db 'X35UMB  '
request dd 0
entry dd 0
handle dw 0
strategy:
    mov [cs:request], bx
    mov [cs:request+2], es
    retf
interrupt:
    pushf
    pusha
    push ds
    push es
    push cs
    pop ds
    les bx, [request]
    mov word [es:bx+3], 0100h
    cmp byte [es:bx+2], 0
    jne done
    mov ax, 4300h
    int 2Fh
    cmp al, 80h
    jne failure
    mov ax, 4310h
    int 2Fh
    mov [entry], bx
    mov [entry+2], es
    mov dx, 0FFFFh
    mov ah, 10h
    call far [entry]
    test ax, ax
    jnz failure
    cmp bl, 0B0h
    jne failure
    mov dx, 16
    mov ah, 10h
    call far [entry]
    cmp ax, 1
    jne failure
    mov [handle], bx
    mov dx, bx
    mov ah, 11h
    call far [entry]
    cmp ax, 1
    jne failure
    mov dx, [handle]
    mov ah, 11h
    call far [entry]
    test ax, ax
    jnz failure
    cmp bl, 0B2h
    jne failure
    mov dx, success
    jmp show
failure:
    mov dx, failed
show:
    mov ah, 09h
    int 21h
    les bx, [request]
    ; Discard the test driver after initialization; no resident resources.
    mov word [es:bx+14], 0
    mov [es:bx+16], cs
done:
    pop es
    pop ds
    popa
    popf
    retf
success db 'S35_XMS_BOOT_UMB_ALLOC_RELEASE_DOUBLE_FREE_OK',13,10,'$'
failed db 'S35_XMS_BOOT_UMB_FAIL',13,10,'$'
