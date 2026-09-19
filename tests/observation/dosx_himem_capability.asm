; S30 default-profile witness.  The runtime's original CONFIG.NT loads
; HIMEM.SYS and its original AUTOEXEC.NT loads DOSX.EXE.  Query their public
; DOS interfaces after the real startup scripts have completed.

bits 16
org 100h

start:
    mov ax, 4300h
    int 2Fh
    cmp al, 80h
    jne failed

    mov ax, 1687h
    int 2Fh
    or ax, ax
    jnz failed

    mov dx, success_text
    mov ah, 09h
    int 21h
    mov ax, 4C00h
    int 21h

failed:
    mov dx, failure_text
    mov ah, 09h
    int 21h
    mov ax, 4C01h
    int 21h

success_text db 'S30_HIMEM_DOSX_OK$'
failure_text db 'S30_HIMEM_DOSX_FAIL$'
