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
    ; INT 67h/57h descriptors follow original emm_fncs.c field order.
    ; Cross the 16 KiB logical-page boundary while both pages are unmapped.
    mov ax,cs
    mov [move_source_segment],ax
    mov [read_dest_segment],ax
    mov ax,[handle]
    mov [move_dest_handle],ax
    mov [read_source_handle],ax
    mov byte [phase],'M'
    mov si,move_request
    xor al,al
    call transfer
    mov byte [phase],'C'
    mov si,read_request
    xor al,al
    call transfer
    cmp word [transfer_result],1357h
    jne mismatch
    cmp word [transfer_result+2],2468h
    jne mismatch
    mov word [transfer_result],369ch
    mov word [transfer_result+2],48adh
    mov byte [phase],'X'
    mov si,read_request
    mov al,1
    call transfer
    cmp word [transfer_result],1357h
    jne mismatch
    cmp word [transfer_result+2],2468h
    jne mismatch
    mov byte [phase],'V'
    mov si,read_request
    xor al,al
    call transfer
    cmp word [transfer_result],369ch
    jne mismatch
    cmp word [transfer_result+2],48adh
    jne mismatch
    ; Conventional addresses may cross independently mapped EMS windows.
    ; Map page 1 before page 0 so host-contiguous copying cannot accidentally
    ; satisfy the test by following the logical backing allocation order.
    mov byte [phase],'W'
    mov bx,1
    xor al,al
    call map
    xor bx,bx
    mov al,1
    call map
    mov es,[frame]
    mov word [es:3ffeh],5b6ch
    mov ax,[frame]
    add ax,400h
    mov es,ax
    mov word [es:0],7d8eh
    mov ax,[frame]
    mov [window_source_segment],ax
    mov ax,cs
    mov [window_dest_segment],ax
    mov si,window_request
    xor al,al
    call transfer
    cmp word [transfer_result],5b6ch
    jne mismatch
    cmp word [transfer_result+2],7d8eh
    jne mismatch
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
transfer:
    mov ah,57h
    int 67h
    or ah,ah
    jnz failed
    ret
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
move_request:
    dd 4
    db 0
    dw 0,transfer_source
move_source_segment dw 0
    db 1
move_dest_handle dw 0
    dw 3ffeh,0
read_request:
    dd 4
    db 1
read_source_handle dw 0
    dw 3ffeh,0
    db 0
    dw 0,transfer_result
read_dest_segment dw 0
window_request:
    dd 4
    db 0
    dw 0,3ffeh
window_source_segment dw 0
    db 0
    dw 0,transfer_result
window_dest_segment dw 0
transfer_source dw 1357h,2468h
transfer_result dw 0,0
passed db 'EMS MAP SWITCH ALIAS UNMAP MOVE EXCHANGE WINDOW FREE PASS',13,10,'$'
noems db 'EMS NOT INSTALLED',13,10,'$'
failmsg db 'EMS FAIL phase/status: $'
newline db 13,10,'$'
