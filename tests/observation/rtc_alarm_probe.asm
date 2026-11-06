; Independent DOS alarm client. Original IRQ8/ROM remains installed.
bits 16
org 100h
    push cs
    pop ds
    mov ax,0x354a
    int 21h
    mov [old_vector],bx
    mov [old_vector+2],es
    mov dx,alarm_handler
    mov ax,0x254a
    int 21h
    cli
    mov al,0x0b
    call cmos_read
    mov [old_b],al
    test al,0x60
    jnz unsupported
    mov al,1
    call cmos_read
    mov [old_sec],al
    mov al,3
    call cmos_read
    mov [old_min],al
    mov al,5
    call cmos_read
    mov [old_hour],al
    in al,0x21
    mov [old_master],al
    and al,0xfb
    out 0x21,al
    in al,0xa1
    mov [old_slave],al
    and al,0xfe
    out 0xa1,al
    mov byte [configured],1
    mov al,1
    mov ah,0xc0
    call cmos_write
    mov al,3
    mov ah,0xc0
    call cmos_write
    mov al,5
    mov ah,0xc0
    call cmos_write
    mov ah,[old_b]
    or ah,0x20
    mov al,0x0b
    call cmos_write
    sti
    call ticks
    mov [start_tick],ax
.wait:
    cmp word [calls],0
    jne .called
    call ticks
    sub ax,[start_tick]
    cmp ax,90
    jb .wait
    jmp cleanup
.called:
    ; Callback disabled AIE; allow time for any duplicate delivery to surface.
    call ticks
    mov [start_tick],ax
.settle:
    call ticks
    sub ax,[start_tick]
    cmp ax,4
    jb .settle
    cli
    cmp word [calls],1
    jne cleanup
    mov al,0x0b
    out 0xa0,al
    in al,0xa0
    test al,1
    jnz cleanup
    mov al,0x0b
    out 0x20,al
    in al,0x20
    test al,4
    jnz cleanup
    mov byte [passed],1
cleanup:
    cli
    cmp byte [configured],0
    je .vector
    mov al,0x0b
    mov ah,[old_b]
    and ah,0xdf
    call cmos_write
    mov al,1
    mov ah,[old_sec]
    call cmos_write
    mov al,3
    mov ah,[old_min]
    call cmos_write
    mov al,5
    mov ah,[old_hour]
    call cmos_write
    mov al,[old_slave]
    out 0xa1,al
    mov al,[old_master]
    out 0x21,al
.vector:
    mov al,0x0f
    out 0x70,al
    sti
    push ds
    lds dx,[old_vector]
    mov ax,0x254a
    int 21h
    pop ds
    cmp byte [passed],1
    jne .fail
    mov ax,0x354a
    int 21h
    cmp bx,[old_vector]
    jne .fail
    mov ax,es
    cmp ax,[old_vector+2]
    jne .fail
    mov al,1
    call cmos_read
    cmp al,[old_sec]
    jne .fail
    mov al,3
    call cmos_read
    cmp al,[old_min]
    jne .fail
    mov al,5
    call cmos_read
    cmp al,[old_hour]
    jne .fail
    mov al,0x0b
    call cmos_read
    cmp al,[old_b]
    jne .fail
    in al,0x21
    cmp al,[old_master]
    jne .fail
    in al,0xa1
    cmp al,[old_slave]
    jne .fail
    mov al,0x0f
    out 0x70,al
    mov dx,success
    mov ah,9
    int 21h
    mov ax,0x4c00
    int 21h
.fail:
    mov dx,failure
    mov ah,9
    int 21h
    mov ax,0x4c01
    int 21h
unsupported:
    jmp cleanup
alarm_handler:
    push ax
    inc word [cs:calls]
    mov al,0x0b
    out 0x70,al
    in al,0x71
    and al,0xdf
    out 0x71,al
    mov al,0x0f
    out 0x70,al
    pop ax
    iret
cmos_read:
    out 0x70,al
    in al,0x71
    ret
cmos_write:
    out 0x70,al
    mov al,ah
    out 0x71,al
    ret
ticks:
    push es
    xor ax,ax
    mov es,ax
    mov ax,[es:0x46c]
    pop es
    ret
old_vector dd 0
start_tick dw 0
calls dw 0
old_b db 0
old_sec db 0
old_min db 0
old_hour db 0
old_master db 0
old_slave db 0
configured db 0
passed db 0
success db 'RTC ALARM INT4A ONCE RETURN EOI RESTORE PASS',13,10,'$'
failure db 'RTC ALARM FAIL OR PREEXISTING RTC OWNER',13,10,'$'
