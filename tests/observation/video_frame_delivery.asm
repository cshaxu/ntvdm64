; Independently authored DOS test, not original guest media. Keep a known
; mode-13 picture alive across the original deferred video-mode/painter ticks.
bits 16
org 100h
    mov ax,0013h
    int 10h
    mov ax,0a000h
    mov es,ax
    xor di,di
    mov cx,32000
    mov al,12h
    rep stosb
    mov cx,32000
    mov al,2ah
    rep stosb
    xor ax,ax
    int 1ah
    mov bx,dx
.wait:
    xor ax,ax
    int 1ah
    sub dx,bx
    cmp dx,55
    jb .wait
    mov ax,0003h
    int 10h
    mov ah,0fh
    int 10h
    cmp al,3
    jne bad_mode
    mov dx,message
    mov ah,9
    int 21h
%ifndef IMMEDIATE_EXIT
    ; This case verifies a displayed text frame as well as copied graphics.
    ; IMMEDIATE_EXIT retains the separate final-output-drain regression case.
    xor ax,ax
    int 1ah
    mov bx,dx
.text_wait:
    xor ax,ax
    int 1ah
    mov [tick_start],bx
    mov [tick_end],dx
    sub dx,bx
    cmp dx,18
    jb .text_wait
%endif
%ifdef TICK_REPORT
    mov dx,tick_path
    xor cx,cx
    mov ah,3ch
    int 21h
    jc bad_mode
    mov bx,ax
    mov dx,tick_start
    mov cx,4
    mov ah,40h
    int 21h
    mov ah,3eh
    int 21h
%endif
    mov ax,4c00h
    int 21h
bad_mode:
    mov ax,4c31h
    int 21h
message db 'GRAPHICS_FRAME_RETURN_OK',13,10,'$'
tick_start dw 0
tick_end dw 0
%ifdef TICK_REPORT
tick_path db 'O:\WINNT\LOGS\VFTICK.BIN',0
%endif
