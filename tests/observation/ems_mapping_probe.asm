; Guest-only EMS integration workload. No host provider or emulator patch.
; Original OpenNT EMS contract: emm_fncs.c services 40h..45h and spcemm.asm.
bits 16
org 100h
    push cs
    pop ds
    mov ax,3567h
    int 21h
    mov ax,es
    or ax,ax
    jz absent
    cmp word [es:0ah],04d45h ; EM in EMMXXXX0
    jne absent
    mov byte [phase],'S'
    mov ah,40h
    int 67h
    or ah,ah
    jnz failed
    mov byte [phase],'F'
    mov ah,41h
    int 67h
    or ah,ah
    jnz failed
    mov [frame],bx
    mov byte [phase],'A'
    mov bx,2
    mov ah,43h
    int 67h
    or ah,ah
    jnz failed
    mov [handle],dx
    mov byte [phase],'0'
    xor bx,bx
    xor al,al
    call map
    mov es,[frame]
    mov word [es:0],0a55ah
    mov byte [phase],'1'
    mov bx,1
    xor al,al
    call map
    mov word [es:0],05aa5h
    mov byte [phase],'R'
    xor bx,bx
    xor al,al
    call map
    cmp word [es:0],0a55ah
    jne mismatch
    mov byte [phase],'B'
    xor bx,bx
    mov al,1
    call map
    mov ax,[frame]
    add ax,400h
    mov es,ax
    cmp word [es:0],0a55ah
    jne mismatch
    mov word [es:0],1234h
    mov es,[frame]
    cmp word [es:0],1234h
    jne mismatch
    mov byte [phase],'U'
    mov bx,0ffffh
    xor al,al
    call map
    mov bx,0ffffh
    mov al,1
    call map
    mov byte [phase],'D'
    mov dx,[handle]
    mov ah,45h
    int 67h
    or ah,ah
    jnz failed
    mov dx,passed
    mov ah,9
    int 21h
    mov ax,4c00h
    int 21h
map:
    mov dx,[handle]
    mov ah,44h
    int 67h
    or ah,ah
    jnz failed
    ret
absent:
    mov dx,noems
    mov ah,9
    int 21h
    mov ax,4c02h
    int 21h
mismatch:
    mov ah,0feh
failed:
    mov [status],ah
    mov dx,failmsg
    mov ah,9
    int 21h
    mov dl,[phase]
    mov ah,2
    int 21h
    mov dl,' '
    int 21h
    mov al,[status]
    mov cl,4
    shr al,cl
    call hex
    mov al,[status]
    and al,0fh
    call hex
    mov dx,newline
    mov ah,9
    int 21h
    mov ax,4c01h
    int 21h
hex:
    add al,'0'
    cmp al,'9'
    jbe .emit
    add al,7
.emit:
    mov dl,al
    mov ah,2
    int 21h
    ret
frame dw 0
handle dw 0
phase db '?'
status db 0
passed db 'EMS MAP SWITCH ALIAS UNMAP FREE PASS',13,10,'$'
noems db 'EMS NOT INSTALLED',13,10,'$'
failmsg db 'EMS FAIL phase/status: $'
newline db 13,10,'$'
