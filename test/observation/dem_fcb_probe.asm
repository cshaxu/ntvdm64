; Authored test-only COM. Real DOS INT21 FCB enumeration, not a replacement
; guest binary or an injected DEM callback. Host owns the isolated directory.
bits 16
org 100h
    push cs
    pop ds
    mov dx, dirname
    mov ah, 3bh
    int 21h
    jc fail
    mov dx, dta
    mov ah, 1ah
    int 21h
    mov dx, fcb
    mov ah, 11h
    int 21h
    test al, al
    jnz fail
    mov di, names
    xor bp, bp
.next:
    inc bp
    cmp bp, 3
    ja fail
    push di
    mov si, dta+1
    mov cx, 11
    push ds
    pop es
    rep movsb
    pop ax
    mov dx, fcb
    mov ah, 12h
    int 21h
    test al, al
    jz .next
    cmp al, 0ffh
    jne fail
    cmp bp, 3
    jne fail
    mov byte [count], 3
    mov byte [ended], al
    mov dx, empty_fcb
    mov ah, 11h
    int 21h
    cmp al, 0ffh
    jne fail
    mov byte [missing], al
    mov dx, trace
    xor cx, cx
    mov ah, 3ch
    int 21h
    jc fail
    mov bx, ax
    mov dx, witness
    mov cx, witness_end-witness
    mov ah, 40h
    int 21h
    jc fail
    cmp ax, witness_end-witness
    jne fail
    mov ah, 3eh
    int 21h
    jc fail
    mov dx, passed
    mov ah, 09h
    int 21h
    mov ax, 4c25h                 ; distinct direct receipt: 37
    int 21h
fail:
    mov ax, 4c63h
    int 21h
dirname db 'Z:\tests\DFCB01',0
trace db 'TRACE.BIN',0
passed db 'DEM-FCB-PASS',13,10,'$'
fcb db 0,'????????','TST'
    times 24 db 0
empty_fcb db 0,'NONE    ','XYZ'
    times 24 db 0
witness db 'FCB1'
count db 0
ended db 0
missing db 0
names times 33 db 0
witness_end:
dta times 128 db 0
