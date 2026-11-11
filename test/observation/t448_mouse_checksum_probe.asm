bits 16
org 0x100
    push cs
    pop ds
    push cs
    pop es
    xor ax,ax
    int 0x33
    cmp ax,0xffff
    jne unavailable
    mov ax,0x15
    int 0x33
    cmp bx,4096
    ja unavailable
    cmp bx,2
    jb unavailable
    mov [size],bx
    mov ax,4
    mov cx,100
    mov dx,40
    int 0x33
    mov ax,3
    int 0x33
    mov [oldx],cx
    mov [oldy],dx
    mov dx,state
    mov ax,0x16
    int 0x33
    xor ax,ax
    mov cx,[size]
    dec cx
    mov si,state
sumloop:
    add al,[si]
    inc si
    loop sumloop
    mov [sum],al
    mov al,[si]
    mov [saved],al
    mov dx,report
    mov ah,9
    int 0x21
    mov al,[sum]
    call hexbyte
    mov dx,report2
    mov ah,9
    int 0x21
    mov al,[saved]
    call hexbyte
    mov dx,newline
    mov ah,9
    int 0x21
    mov ax,4
    mov cx,200
    mov dx,100
    int 0x33
    mov ax,3
    int 0x33
    cmp cx,[oldx]
    jne changed
    cmp dx,[oldy]
    je unavailable
changed:
    mov [newx],cx
    mov [newy],dx
    ; Corrupt only the saved checksum and verify restore leaves state alone.
    mov bx,[size]
    dec bx
    xor byte [state+bx],1
    mov dx,state
    mov ax,0x17
    int 0x33
    mov ax,3
    int 0x33
    cmp cx,[newx]
    jne fail
    cmp dx,[newy]
    jne fail
    mov bx,[size]
    dec bx
    xor byte [state+bx],1
    ; A bad cookie is rejected even with a matching recomputed checksum.
    inc byte [state]
    inc byte [state+bx]
    mov dx,state
    mov ax,0x17
    int 0x33
    mov ax,3
    int 0x33
    cmp cx,[newx]
    jne fail
    cmp dx,[newy]
    jne fail
    dec byte [state]
    mov bx,[size]
    dec bx
    dec byte [state+bx]
    mov dx,state
    mov ax,0x17
    int 0x33
    mov ax,3
    int 0x33
    cmp cx,[oldx]
    jne fail
    cmp dx,[oldy]
    jne fail
    mov dx,passmsg
    mov ah,9
    int 0x21
    mov ax,0x4c00
    int 0x21
fail:
    mov dx,failmsg
    mov ah,9
    int 0x21
    ; Diagnostic control: modify only our own test input buffer, not driver
    ; state or code, and check whether the documented checksum gates restore.
    mov bx,[size]
    dec bx
    mov al,[sum]
    mov [state+bx],al
    mov dx,state
    mov ax,0x17
    int 0x33
    mov ax,3
    int 0x33
    cmp cx,[oldx]
    jne controlfail
    cmp dx,[oldy]
    jne controlfail
    mov dx,controlpassmsg
    mov ah,9
    int 0x21
    mov ax,0x4c01
    int 0x21
controlfail:
    mov dx,controlfailmsg
    mov ah,9
    int 0x21
    mov ax,0x4c01
    int 0x21
unavailable:
    mov dx,unmsg
    mov ah,9
    int 0x21
    mov ax,0x4c02
    int 0x21
hexbyte:
    push ax
    mov cl,4
    shr al,cl
    call nibble
    pop ax
    and al,15
nibble:
    and al,15
    add al,'0'
    cmp al,'9'
    jbe print
    add al,7
print:
    mov dl,al
    mov ah,2
    int 0x21
    ret
size: dw 0
oldx: dw 0
oldy: dw 0
newx: dw 0
newy: dw 0
sum: db 0
saved: db 0
report: db 'SUM=', '$'
report2: db ' SAVED=', '$'
newline: db 13,10,'$'
passmsg: db 'MOUSE-STATE-PASS',13,10,'$'
failmsg: db 'MOUSE-STATE-FAIL',13,10,'$'
controlpassmsg: db 'CHECKSUM-CONTROL-PASS',13,10,'$'
controlfailmsg: db 'CHECKSUM-CONTROL-FAIL',13,10,'$'
unmsg: db 'MOUSE-STATE-UNAVAILABLE',13,10,'$'
state: times 4096 db 0
