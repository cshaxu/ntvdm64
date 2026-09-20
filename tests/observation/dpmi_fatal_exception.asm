; Independent fault probe; never changes original DOSX or other guest media.
; Leave exception 13 at its original handler and load an invalid selector.
; The host must present EG_BAD_FAULT with X#=0D, not silently complete.
bits 16
org 100h
start:
    mov sp, stack_top
    mov bx, (image_end - $$ + 100h + 15) / 16
    mov ah, 4Ah
    int 21h
    jc failed
    mov ax, 1687h
    int 2Fh
    test ax, ax
    jnz failed
    mov [entry], di
    mov [entry+2], es
    mov bx, si
    test bx, bx
    jz enter_pm
    mov ah, 48h
    int 21h
    jc failed
    mov es, ax
enter_pm:
%ifdef CLIENT32
    mov ax, 1
%else
    xor ax, ax
%endif
    call far [entry]
    jc failed
    mov edx, ready_message
    mov ah, 09h
    int 21h
    mov ax, 0FFFFh
fault_instruction:
    mov ds, ax
    ; Reaching here without a handler changing the faulting state is failure.
failed:
    mov ax, 4C63h
    int 21h
    jmp $
entry: dd 0
ready_message: db 'S38_FATAL_GP_READY',13,10,'$'
align 16
    times 1024 db 0
stack_top:
image_end:
