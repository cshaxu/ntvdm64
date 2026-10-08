; Test-only DOS COM witness for the original protected direct-hard-disk route.
; It issues the same INT 13h read class that the Windows 3.1 Setup hardware
; scan reaches.  After the host's explicit Ignore response, the original BIOS
; contract is CF=1/AH=80h and normal execution of this COM image continues.

bits 16
org 100h

start:
    mov ax, 0201h             ; read one sector
    mov bx, buffer
    mov cx, 0001h             ; cylinder 0, sector 1
    mov dx, 0080h             ; first hard disk
    int 13h
    jc direct_access_failed

    mov dx, unexpected_success
    mov ax, 4c01h
    jmp short print_and_exit

direct_access_failed:
    cmp ah, 80h
    jne wrong_failure
    mov dx, expected_failure
    mov ax, 4c00h
    jmp short print_and_exit

wrong_failure:
    mov dx, wrong_failure_text
    mov ax, 4c02h

print_and_exit:
    push ax
    mov ah, 09h
    int 21h
    pop ax
    int 21h

expected_failure db 'D13_IGNORE_RETURNS_CF_AH80$'
unexpected_success db 'D13_UNEXPECTED_SUCCESS$'
wrong_failure_text db 'D13_WRONG_FAILURE$'
buffer times 512 db 0
