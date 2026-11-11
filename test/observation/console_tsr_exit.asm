; Independent DOS test input, not a replacement for any guest component.
; Returning resident reaches COMMAND's original TSR prompt/exit handling.
bits 16
org 100h
    mov dx, ready
    mov ah, 09h
    int 21h
    mov dx, (image_end-$$+100h+15)/16
    mov ax, 3100h
    int 21h
ready: db 'CONSOLE-TSR-READY',13,10,'$'
image_end:
