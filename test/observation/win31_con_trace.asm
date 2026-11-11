; Independent DOS TSR observation only. Chain all original DOS/multiplex calls.
; No original guest bytes patched. Reports exact CON opens and their outcomes.
bits 16
org 100h
jmp main
old21 dd 0
old2f dd 0
old10 dd 0
attempts dw 0
successes dw 0
failures dw 0
last_result dw 0
logchar:
    push bx
    mov bx,[cs:log_length]
    cmp bx,4096
    jae log_full
    mov [cs:log_data+bx],al
    inc bx
    mov [cs:log_length],bx
log_full:
    pop bx
    ret
int10:
    pushf
    push ax
    cmp ah,0eh
    jne bios_chain
    call logchar
bios_chain:
    pop ax
    popf
    jmp far [cs:old10]
int21:
    pushf
    push ax
    push bx
    push cx
    push si
    cmp ah,2
    jne log_string
    mov al,dl
    call logchar
    jmp logged
log_string:
    cmp ah,9
    jne log_write
    mov si,dx
    mov cx,512
log_dollar:
    mov al,[si]
    cmp al,'$'
    je logged
    call logchar
    inc si
    loop log_dollar
    jmp logged
log_write:
    cmp ah,40h
    jne logged
    cmp bx,2
    ja logged
    cmp bx,1
    jb logged
    mov si,dx
    jcxz logged
    cmp cx,512
    jbe log_buffer
    mov cx,512
log_buffer:
    mov al,[si]
    call logchar
    inc si
    loop log_buffer
logged:
    pop si
    pop cx
    mov bx,sp
    mov ax,[ss:bx+2]
    cmp ah,3dh
    jne passthrough
    mov bx,dx
    cmp word [bx],4f43h
    jne passthrough
    cmp byte [bx+2],'N'
    jne passthrough
    cmp byte [bx+3],0
    jne passthrough
    inc word [cs:attempts]
    pop bx
    pop ax
    popf
    pushf
    call far [cs:old21]
    pushf
    push ax
    push bx
    mov [cs:last_result],ax
    mov bx,sp
    test word [ss:bx+4],1
    jnz failed
    inc word [cs:successes]
    jmp short restored
failed:
    inc word [cs:failures]
restored:
    pop bx
    pop ax
    popf
    retf 2
passthrough:
    pop bx
    pop ax
    popf
    jmp far [cs:old21]
int2f:
    cmp ax,0e731h
    jne chain2f
    cmp bx,4e54h
    jne chain2f
    mov ax,4e54h
    mov bx,[cs:attempts]
    mov cx,[cs:successes]
    mov dx,[cs:failures]
    mov si,[cs:last_result]
    iret
chain2f:
    jmp far [cs:old2f]
log_marker db 'T436-GUESTLOG-v1'
log_length dw 0
log_data times 4096 db 0
resident_end:
label db 'CONTRACE attempts/success/failure/last=', '$'
newline db 13,10,'$'
main:
    cmp byte [80h],0
    jne dump
    mov ax,3510h
    int 21h
    mov [old10],bx
    mov [old10+2],es
    mov ax,3521h
    int 21h
    mov [old21],bx
    mov [old21+2],es
    mov ax,352fh
    int 21h
    mov [old2f],bx
    mov [old2f+2],es
    mov dx,int21
    mov ax,2521h
    int 21h
    mov dx,int2f
    mov ax,252fh
    int 21h
    mov dx,int10
    mov ax,2510h
    int 21h
    mov dx,(resident_end-$$+100h+15)/16
    mov ax,3100h
    int 21h
dump:
    mov ax,0e731h
    mov bx,4e54h
    int 2fh
    cmp ax,4e54h
    jne absent
    push si
    push dx
    push cx
    push bx
    mov dx,label
    mov ah,9
    int 21h
    mov cx,4
print_loop:
    pop ax
    push cx
    call hex
    pop cx
    mov dl,' '
    mov ah,2
    int 21h
    loop print_loop
    mov dx,newline
    mov ah,9
    int 21h
    mov ax,4c00h
    int 21h
absent:
    mov ax,4c01h
    int 21h
hex:
    mov bx,ax
    mov cx,4
hex_loop:
    rol bx,4
    mov dl,bl
    and dl,15
    add dl,'0'
    cmp dl,'9'
    jbe emit
    add dl,7
emit:
    mov ah,2
    int 21h
    loop hex_loop
    ret
