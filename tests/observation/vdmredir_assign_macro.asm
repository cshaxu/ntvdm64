; Real DOS legacy assignment lifecycle through the original redirector:
; DefineMacro (5F03), GetAssignList (5F02), then BreakMacro (5F04).
bits 16
org 100h

start:
    push cs
    pop ds
    push cs
    pop es

    ; O$ is a local administrative share supplied by the test host.  Z: is
    ; checked free before the observer starts this probe.
    mov si, local_name
    mov di, remote_name
    mov cx, 0beefh
    mov bl, 4                    ; disk device
    mov ax, 5f03h
    int 21h
    jnc define_ok
    mov byte [stage], 'A'
    jmp failure
define_ok:

    xor bx, bx                   ; original list is sorted; Z: need not be first
.find_z:
    mov si, returned_local
    mov di, returned_remote
    mov ax, 5f02h
    int 21h
    jc get_not_found
    cmp byte [returned_local], 'Z'
    je .found_z
    inc bx
    cmp bx, 26
    jb .find_z
    mov byte [stage], 'N'
    jmp get_fail
.found_z:
    cmp bl, 4
    jne type_fail
    cmp cx, 0beefh
    jne user_fail
    cmp byte [returned_local], 'Z'
    jne local_fail
    cmp byte [returned_local+1], ':'
    jne local_fail
    mov si, returned_remote
    mov di, remote_name
.compare:
    mov al, [si]
    cmp al, [di]
    jne remote_fail
    inc si
    inc di
    or al, al
    jnz .compare

    mov si, local_name
    mov ax, 5f04h
    int 21h
    jnc break_ok
    mov byte [stage], 'D'
    jmp failure
break_ok:
    mov dx, success
    mov cx, success_end-success
    mov bx, 1
    mov ah, 40h
    int 21h
    mov ax, 4c00h
    int 21h

get_fail:
    ; DefineMacro completed, so preserve a clean host state after a failed
    ; query as well.
    mov si, local_name
    mov ax, 5f04h
    int 21h
failure:
    mov dx, failed
    mov cx, failed_end-failed
    mov bx, 1
    mov ah, 40h
    int 21h
    mov ax, 4c01h
    int 21h

get_not_found:
    mov byte [stage], 'G'
    jmp get_fail
type_fail:
    mov byte [stage], 'T'
    jmp get_fail
user_fail:
    mov byte [stage], 'U'
    jmp get_fail
local_fail:
    mov byte [stage], 'L'
    jmp get_fail
remote_fail:
    mov byte [stage], 'R'
    jmp get_fail

local_name db 'Z:',0
; VrpTranslateDosNetPath intentionally returns an uppercase DOS net path.
remote_name db '\\LOCALHOST\O$',0
returned_local times 16 db 0
returned_remote times 128 db 0
success db 'ASSIGN-MACRO-LIFECYCLE-OK',13,10
success_end:
failed db 'ASSIGN-MACRO-LIFECYCLE-FAIL stage='
stage db '?',13,10
failed_end:
