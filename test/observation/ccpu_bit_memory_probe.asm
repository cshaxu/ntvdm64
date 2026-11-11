; Independent BTC word memory witness with both possible addresses in bounds.
bits 16
org 100h
jmp start

start:
    push cs
    pop ds
    mov word [anchor],0x8000
    mov bx,15
    a32 btc word [anchor],bx
    setc byte [carry]
    cmp byte [carry],1
    jne positive_failed
    cmp word [anchor],0
    jne positive_failed
    mov dx,positive_message
    mov ah,9
    int 21h
    mov word [anchor-2],0x8000
    mov word [anchor+8190],0
    mov bx,0xffff
    a32 btc word [anchor],bx
    setc byte [carry]
    mov dx,near_message
    mov ah,9
    int 21h
    mov ax,[anchor-2]
    call hex_word
    mov dx,far_message
    mov ah,9
    int 21h
    mov ax,[anchor+8190]
    call hex_word
    mov dx,carry_message
    mov ah,9
    int 21h
    mov dl,[carry]
    add dl,'0'
    mov ah,2
    int 21h
    mov dx,newline
    mov ah,9
    int 21h
    cmp word [anchor-2],0
    jne negative_failed
    cmp word [anchor+8190],0
    jne negative_failed
    cmp byte [carry],1
    jne negative_failed
    mov dx,pass_message
    mov ah,9
    int 21h
    mov ax,4c00h
    int 21h
positive_failed:
    mov dx,positive_failure
    jmp failed
negative_failed:
    mov dx,negative_failure
failed:
    mov ah,9
    int 21h
    mov ax,4c01h
    int 21h

hex_word:
    push ax
    push bx
    push cx
    push dx
    mov bx,ax
    mov cx,4
.digit:
    rol bx,4
    mov dl,bl
    and dl,15
    add dl,'0'
    cmp dl,'9'
    jbe .emit
    add dl,7
.emit:
    mov ah,2
    int 21h
    loop .digit
    pop dx
    pop cx
    pop bx
    pop ax
    ret

carry db 0
positive_message db 'BITMEM POSITIVE PASS',13,10,'$'
near_message db 'BITMEM near=','$'
far_message db ' far=','$'
carry_message db ' carry=','$'
newline db 13,10,'$'
pass_message db 'BITMEM NEGATIVE PASS',13,10,'$'
positive_failure db 'BITMEM POSITIVE FAIL',13,10,'$'
negative_failure db 'BITMEM NEGATIVE FAIL',13,10,'$'
align 2
dw 0
anchor dw 0
times 8190 db 0
dw 0
