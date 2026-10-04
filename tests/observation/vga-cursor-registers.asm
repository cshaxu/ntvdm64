; Authored probe: only CRTC registers change between observed states.
bits 16
org 100h
    mov ax,3
    int 10h
    mov dx,ready
    mov ah,9
    int 21h
.key:
    xor ah,ah
    int 16h
    or al,20h
    cmp al,'t'
    jne .key
    mov bx,0334h                 ; (20,10), 80 columns
    call position
    mov ax,020ah                ; start scan line 2
    call register
    mov ax,050bh                ; end scan line 5
    call register
    call settle
    mov ax,1f0ah                ; original do_new_cursor hides start >= char height
    call register
    call settle
    mov bx,0335h                 ; (21,10), no character write
    call position
    mov ax,020ah
    call register
    call settle
    mov dx,passed
    mov ah,9
    int 21h
    mov ax,4c00h
    int 21h
position:
    mov ah,bh
    mov al,0eh
    call register
    mov ah,bl
    mov al,0fh
    call register
    ret
register:
    mov dx,3d4h
    out dx,ax
    ret
settle:
    xor ah,ah
    int 1ah
    mov bp,dx
.wait:
    xor ah,ah
    int 1ah
    sub dx,bp
    cmp dx,8
    jb .wait
    ret
ready db 'S7_TEXT_CURSOR_READY',13,10,'$'
passed db 'CURSOR-REGISTERS-PASS',13,10,'$'
