; S30 test-only LPT1 witness.  The observer temporarily maps the Windows
; LPT1 DOS device to its pipe before this guest starts; product code still
; follows the original INT 17 -> nt_lpt CreateFile("LPT1") path.

bits 16
org 100h

start:
    mov ax, 005ah
    xor dx, dx
    int 17h
    test ah, 01h
    jnz failed
    mov dx, success_text
    mov ah, 09h
    int 21h
    mov ax, 4c00h
    int 21h

failed:
    mov dx, failure_text
    mov ah, 09h
    int 21h
    mov ax, 4c01h
    int 21h

success_text db 'S30_LPT1_WRITE_OK$'
failure_text db 'S30_LPT1_WRITE_FAIL$'
