; Test-only DOS COM witness for the current no-attached-media disks profile.
; It uses original INT 13h dispatch only.  No handler, disk image or guest
; medium is installed or changed.

bits 16
org 100h

start:
    ; Original diskbios reset is a valid controller operation even with no
    ; configured backing store.
    mov dx, step_reset
    mov ah, 09h
    int 21h
    mov ah, 00h
    mov dl, 80h
    int 13h
    jc fail_reset

    mov dx, success_text
    jmp short emit
fail_reset:  mov dx, reset_failure
fail:
    mov ah, 09h
    int 21h
    mov ax, 4c01h
    int 21h
emit:
    mov ah, 09h
    int 21h
    mov ax, 4c00h
    int 21h

step_reset db 'S28_STEP_RESET $'
success_text db 'S28_INT13_RESET_OK S28_DISKS_OK$'
reset_failure db 'S28_INT13_RESET_FAIL$'
