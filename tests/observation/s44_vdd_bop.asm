; Disposable S44 guest probe.  It invokes the original MS BOP 58 modes:
; register a native provider, dispatch it, then deregister it.  It is a test
; input under build/, never a replacement or modification of guest media.
bits 16
org 100h

start:
    push cs
    pop ds
    push cs
    pop es
    mov si, dll_name
    mov di, init_name
    mov bx, dispatch_name
    db 0c4h, 0c4h, 058h, 0       ; RegisterModule
    jc failed
    mov [handle], ax
    db 0c4h, 0c4h, 058h, 2       ; DispatchCall
    mov dx, 0e9h
    in al, dx                    ; original VDDInstallIOHook input route
    out dx, al                   ; original VDDInstallIOHook output route
    mov ax, [handle]
    db 0c4h, 0c4h, 058h, 1       ; DeRegisterModule
    mov dx, succeeded
    mov ah, 9
    int 21h
    xor al, al
    jmp short done

failed:
    mov bx, ax
    mov al, bh
    shr al, 4
    call hex_nibble
    mov [failed_code], al
    mov al, bh
    call hex_nibble
    mov [failed_code+1], al
    mov al, bl
    shr al, 4
    call hex_nibble
    mov [failed_code+2], al
    mov al, bl
    call hex_nibble
    mov [failed_code+3], al
    mov dx, failed_message
    mov ah, 9
    int 21h
    mov al, 1

done:
    mov ah, 4ch
    int 21h

hex_nibble:
    and al, 0fh
    add al, '0'
    cmp al, '9'
    jbe short hex_done
    add al, 7
hex_done:
    ret

handle dw 0
dll_name db 'tests\\s44-bop-vdd.dll',0
; The test DLL has a source-shaped `.def`-style export alias, matching the
; ordinary original VDD samples and their GetProcAddress input strings.
init_name db 'S44BOPInit',0
dispatch_name db 'S44BOPDispatch',0
succeeded db 'S44BOP OK',13,10,'$'
failed_message db 'S44BOP FAIL AX='
failed_code db '0000',13,10,'$'
