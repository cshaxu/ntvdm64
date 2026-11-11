; Real DOS local NetUse lifecycle through the original 16-bit redirector:
; add a non-persistent IPC$ use, enumerate it, query it, then delete it.
bits 16
org 100h

start:
    push cs
    pop ds
    push cs
    pop es
    ; REM16_use_info_1 = B9BzzWWWW: fixed nine-byte local device,
    ; one-byte alignment, then the two far strings and four words.
    mov word [use1+10], remote
    mov word [use1+12], ds        ; ui1_remote
    ; XsNetUseAdd reselects the fixed REM16_use_info_1 descriptor, so the
    ; second ``z`` must be present in the packed wire buffer: an empty
    ; password is therefore a far pointer to an empty string here.
    mov word [use1+14], empty_password
    mov word [use1+16], ds        ; ui1_password = empty string
    mov word [use1+20], 3         ; ui1_asg_type = USE_IPC
    mov bx, 1                     ; original VrNetUseAdd requires USE_INFO_1
    mov cx, 28
    ; The documented local-server value is the null *far* pointer 0000:0000.
    ; CS:0000 is program memory, not a null pointer.
    xor ax, ax
    mov ds, ax
    xor si, si
    mov di, use1
    mov ax, 5f47h
    int 21h
    push cs
    pop ds
    jnc add_ok
    mov byte [stage], 'A'
    jmp failure
add_ok:

    xor bx, bx
    mov cx, 128
    mov di, enum_buffer
    mov ax, 5f46h
    int 21h
    jnc enum_ok
    mov byte [stage], 'E'
    jmp cleanup_failure
enum_ok:
    or cx, cx
    jnz enum_count_ok
    mov byte [stage], 'N'
    xor ax, ax
    jmp cleanup_failure
enum_count_ok:

    mov word [get_info+0], remote
    mov word [get_info+2], ds
    mov word [get_info+4], 0
    mov word [get_info+6], info_buffer
    mov word [get_info+8], ds
    mov word [get_info+10], 128
    mov dx, get_info
    mov ax, 5f49h
    int 21h
    jnc info_ok
    mov byte [stage], 'I'
    jmp cleanup_failure
info_ok:

    xor si, si
    mov di, remote
    xor bx, bx
    mov ax, 5f48h
    int 21h
    jnc del_ok
    mov byte [stage], 'D'
    jmp failure
del_ok:
    mov dx, success
    mov cx, success_end-success
    mov bx, 1
    mov ah, 40h
    int 21h
    mov ax, 4c00h
    int 21h

cleanup_failure:
    mov [error_code], ax
    xor si, si
    mov di, remote
    xor bx, bx
    mov ax, 5f48h
    int 21h
    jmp failure_saved
failure:
    mov [error_code], ax
failure_saved:
    mov ax, [error_code]
    mov di, error_hex+3
    mov cx, 4
.hex:
    mov bx, ax
    and bx, 0fh
    add bl, '0'
    cmp bl, '9'
    jbe .store
    add bl, 7
.store:
    mov [di], bl
    dec di
    shr ax, 4
    loop .hex
    mov dx, failed
    mov cx, failed_end-failed
    mov bx, 1
    mov ah, 40h
    int 21h
    mov ax, 4c01h
    int 21h

remote db '\\localhost\IPC$',0
empty_password db 0
error_code dw 0
use1 times 28 db 0
get_info times 12 db 0
enum_buffer times 128 db 0
info_buffer times 128 db 0
success db 'NETUSE-LIFECYCLE-OK',13,10
success_end:
failed db 'NETUSE-LIFECYCLE-FAIL stage='
stage db '?'
       db ' ax='
error_hex db '0000',13,10
failed_end:
