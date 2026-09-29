; Independent disposable COM probe, never a replacement for original media.
bits 16
org 100h
    xor ax,ax
    mov si,81h
skip_space:
    lodsb
    cmp al,' '
    je skip_space
    xor bx,bx
digit:
    cmp al,'0'
    jb parsed
    cmp al,'9'
    ja parsed
    sub al,'0'
    xor ah,ah
    mov dx,bx
    shl bx,1
    shl dx,3
    add bx,dx
    add bx,ax
    lodsb
    jmp digit
parsed:
    mov [expected],bx
    ; Original spckbd.asm sw_video_io defers VGA initialization while the
    ; application uses stream output only. Querying (not setting) the video
    ; mode follows its normal sw_mode_change -> ConsoleInit path before we
    ; inspect BIOS geometry. Do not mistake dormant boot BDA for an active mode.
    mov ah,0fh
    int 10h
    mov ax,40h
    mov es,ax
    xor ax,ax
    mov al,[es:84h]
    inc ax
    mov [observed],ax
    cmp word [es:4ah],80
    jne failed
    cmp ax,[expected]
    jne failed
    mov dx,passed
    mov ah,9
    int 21h
    mov ax,4c00h
    int 21h
failed:
    mov dx,failure
    mov ah,9
    int 21h
    mov ax,[observed]
    mov bl,10
    div bl
    mov bx,ax
    mov dl,bl
    add dl,'0'
    mov ah,2
    int 21h
    mov dl,bh
    add dl,'0'
    int 21h
    mov ax,4c07h
    int 21h
expected dw 0
observed dw 0
passed db 'S13-GUEST-GRID-PASS',13,10,'$'
failure db 'S13-GUEST-GRID-FAIL actual rows=','$'
