; Original redirector INT 2Fh/1184h NetGetEnumInfo coverage.
; Computer and user names must publish a guest buffer.  The original OpenNT
; host deliberately reports ERROR_NOT_SUPPORTED for primary-domain and
; logon-server requests; this probe verifies that failure shape rather than
; inventing a replacement network service.
bits 16
org 100h

start:
    push cs
    pop ds
    push cs
    pop es

    ; Selector 0 is the private CDNames triple.  It receives three 16:16
    ; output destinations in ES:DI; the standalone boundary must resolve
    ; those through the worker-owned bounded copy binding.
    mov word [cd_names+0], cd_computer
    mov ax, ds
    mov word [cd_names+2], ax
    mov word [cd_names+4], cd_primary_domain
    mov word [cd_names+6], ax
    mov word [cd_names+8], cd_logon_domain
    mov word [cd_names+10], ax
    xor bx, bx
    mov di, cd_names
    mov ax, 1184h
    int 2fh
    push cs
    pop ds
    cmp byte [cd_computer], 0
    je failure

    mov ax, 1184h
    mov bx, 3                    ; ComputerName selector
    mov di, computer
    int 2fh
    jc failure
    push cs
    pop ds
    cmp byte [computer], 0
    je failure

    push cs
    pop es
    mov ax, 1184h
    mov bx, 4                    ; UserName selector
    mov di, user_name
    int 2fh
    jc failure
    push cs
    pop ds
    cmp byte [user_name], 0
    je failure

    push cs
    pop es
    mov ax, 1184h
    mov bx, 5                    ; original unsupported DomainName
    mov di, domain
    int 2fh
    jnc failure

    push cs
    pop es
    mov ax, 1184h
    mov bx, 6                    ; original unsupported LogonServer
    mov di, logon_server
    int 2fh
    jnc failure

    push cs
    pop ds
    mov dx, success
    mov cx, success_end-success
    mov bx, 1
    mov ah, 40h
    int 21h
    mov ax, 4c00h
    int 21h

failure:
    push cs
    pop ds
    mov dx, failed
    mov cx, failed_end-failed
    mov bx, 1
    mov ah, 40h
    int 21h
    mov ax, 4c01h
    int 21h

computer     times 64 db 0
user_name    times 64 db 0
domain       times 64 db 0
logon_server times 64 db 0
cd_names times 12 db 0
cd_computer times 64 db 0
cd_primary_domain times 64 db 0
cd_logon_domain times 64 db 0
success db 'NETENUM-OK',13,10
success_end:
failed db 'NETENUM-FAIL',13,10
failed_end:
