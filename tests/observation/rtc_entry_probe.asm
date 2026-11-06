; Independent DOS probe of mutable INT12 BDA and original INT15 wait dispatch.
; No original guest image or interrupt vector is replaced.
bits 16
org 100h
    push cs
    pop ds
    xor ax,ax
    mov es,ax
    int 12h
    cmp ax,[es:0x413]
    jne failed
    mov [old_memory],ax
    pushf
    cli
    dec ax
    mov [es:0x413],ax
    int 12h
    inc ax
    cmp ax,[old_memory]
    setne byte [memory_mismatch]
    mov ax,[old_memory]
    mov [es:0x413],ax
    popf
    cmp byte [memory_mismatch],0
    jne failed
    int 12h
    cmp ax,[old_memory]
    jne failed
    mov dx,memory_ok
    mov ah,9
    int 21h
    mov bx,0x15*4
    call vector_bytes
    mov bx,0x70*4
    call vector_bytes
    ; Original INT15/83 is supplied beyond the installed wrappers.
    push cs
    pop es
    mov byte [wait_flag],0x35
    mov bx,wait_flag
    mov cx,1
    mov dx,0x86a0 ; 100000 microseconds
    mov ax,0x8300
    int 15h
    jc failed_cancel
    call ticks
    mov [start_tick],ax
.wait:
    test byte [wait_flag],0x80
    jnz .done
    call ticks
    sub ax,[start_tick]
    cmp ax,36
    jb .wait
    jmp failed_cancel
.done:
    cmp byte [wait_flag],0xb5
    jne failed_cancel
    ; Pure Intel RTC decrements by 976 and retains the original flag bits.
    push es
    xor ax,ax
    mov es,ax
    mov eax,[es:0x49c]
    pop es
    cmp eax,0xfffffc30
    jb failed_cancel
    test eax,eax
    jns failed_cancel
    mov dx,wait_ok
    mov ah,9
    int 21h
    mov cx,0
    mov dx,10000
    mov ah,0x86
    int 15h
    jc failed
    mov dx,sync_ok
    mov ah,9
    int 21h
    ; Cancel a longer wait; it must not complete after cancellation.
    mov byte [wait_flag],0x35
    push cs
    pop es
    mov bx,wait_flag
    mov cx,15
    mov dx,0x4240
    mov ax,0x8300
    int 15h
    jc failed_cancel
    mov ax,0x8301
    int 15h
    jc failed
    call ticks
    mov [start_tick],ax
.cancel_wait:
    call ticks
    sub ax,[start_tick]
    cmp ax,3
    jb .cancel_wait
    cmp byte [wait_flag],0x35
    jne failed
    mov al,0x0b
    out 0x70,al
    in al,0x71
    test al,0x40
    jnz failed
    mov al,0x0f
    out 0x70,al
    ; Original cancel leaves the BDA busy bit when DS is caller data.
    push es
    xor ax,ax
    mov es,ax
    test byte [es:0x4a0],1
    pop es
    jz failed
    mov bx,wait_flag
    xor cx,cx
    mov dx,1
    mov ax,0x8300
    int 15h
    jnc failed_cancel
    ; Original in-use branch guarantees CF; it does not set AH=86h.
    mov [result_ax],ax
    mov dx,cancel_limit
    mov ah,9
    int 21h
    mov dx,cancel_ok
    mov ah,9
    int 21h
    mov ax,0x4c00
    int 21h
failed_cancel:
    mov ax,0x8301
    int 15h
failed:
    mov al,[phase]
    call hexbyte
    mov ax,[result_ax]
    mov al,ah
    call hexbyte
    mov al,[result_ax]
    call hexbyte
    mov al,[result_flags]
    call hexbyte
    mov dx,failure
    mov ah,9
    int 21h
    mov ax,0x4c01
    int 21h
hexbyte:
    push ax
    shr al,4
    call nibble
    pop ax
    and al,15
nibble:
    add al,'0'
    cmp al,'9'
    jbe .emit
    add al,7
.emit:
    mov dl,al
    mov ah,2
    int 21h
    ret
ticks:
    push es
    xor ax,ax
    mov es,ax
    mov ax,[es:0x46c]
    pop es
    ret
vector_bytes:
    push es
    push si
    xor ax,ax
    mov es,ax
    mov si,[es:bx]
    mov ax,[es:bx+2]
    mov [vector_seg],ax
    mov al,ah
    call hexbyte
    mov al,[vector_seg]
    call hexbyte
    mov ax,si
    mov al,ah
    call hexbyte
    mov ax,si
    call hexbyte
    mov es,[vector_seg]
    mov cx,8
.loop:
    mov al,[es:si]
    call hexbyte
    inc si
    loop .loop
    mov dx,newline
    mov ah,9
    int 21h
    cmp bx,0x70*4
    jne .return
    ; The witnessed NTIO body is CALL do_int_stacks; DW old70.
    sub si,8
    cmp byte [es:si],0xe8
    jne .return
    mov bx,[es:si+3]
    mov si,[es:bx]
    mov ax,[es:bx+2]
    mov [vector_seg],ax
    mov al,ah
    call hexbyte
    mov al,[vector_seg]
    call hexbyte
    mov ax,si
    mov al,ah
    call hexbyte
    mov ax,si
    call hexbyte
    mov es,[vector_seg]
    mov cx,8
.chain:
    mov al,[es:si]
    call hexbyte
    inc si
    loop .chain
    mov dx,newline
    mov ah,9
    int 21h
.return:
    pop si
    pop es
    ret
old_memory dw 0
vector_seg dw 0
result_ax dw 0
result_flags dw 0
phase db 0x83
wait_flag db 0
memory_mismatch db 0
memory_ok db 'RTC AUDIT MUTABLE INT12 BDA PASS',13,10,'$'
start_tick dw 0
wait_ok db 'RTC ASYNC WAIT COMPLETE 976 FLAG PRESERVED',13,10,'$'
cancel_limit db 'RTC ORIGINAL CANCEL RETAINS BUSY LIMIT',13,10,'$'
cancel_ok db 'RTC CANCEL NO COMPLETION PASS',13,10,'$'
sync_ok db 'RTC SYNC WAIT COMPLETE PASS',13,10,'$'
failure db 'RTC ENTRY AUDIT DIFFERENT CONTRACT',13,10,'$'
newline db 13,10,'$'
