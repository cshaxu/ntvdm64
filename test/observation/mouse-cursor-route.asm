; Authored probe: displayed in Console, stationary ENTER, then source LEAVE.
bits 16
org 100h
    mov ax,3
    int 10h
    xor ax,ax
    int 33h
    cmp ax,0ffffh
    jne fail
    mov ax,0ah
    xor bx,bx
    mov cx,0ffffh
    mov dx,7700h
    int 33h
    mov ax,7                    ; constrain motion to a stationary guest point
    mov cx,160
    mov dx,cx
    int 33h
    mov ax,8
    mov cx,80
    mov dx,cx
    int 33h
    mov ax,4
    mov cx,160
    mov dx,80
    int 33h
    mov ax,1
    int 33h
    mov dx,ready
    mov ah,9
    int 21h
.key:
    xor ah,ah
    int 16h
    or al,20h
    cmp al,'t'
    jne .key
    mov ax,0b800h
    mov es,ax
    call settle
    cmp word [es:1640],7020h
    jne fail
    xor ah,ah
    int 1ah
    mov bp,dx
.leave:
    cmp word [es:1640],0720h
    je passed
    xor ah,ah
    int 1ah
    sub dx,bp
    cmp dx,180
    jb .leave
fail:
    mov dx,failed
    mov bx,1
    jmp finish
passed:
    call settle                 ; let the unchanged framebuffer be observed
    mov dx,success
    xor bx,bx
finish:
    mov ah,9
    int 21h
    mov ax,bx
    mov ah,4ch
    int 21h
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
success db 'CURSOR-ROUTE-PASS',13,10,'$'
failed db 'CURSOR-ROUTE-FAIL',13,10,'$'
