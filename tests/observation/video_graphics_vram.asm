; Test-only DOS COM guest for the selected graphics memory provider.  It does
; not claim public-Console graphics presentation: after the direct A000h
; read/write check it returns to original mode 3 before emitting its marker.

bits 16
org 100h

start:
    mov ax, 0013h                 ; original VGA 320x200x256 mode
    int 10h
    mov ax, 0a000h
    mov es, ax
    xor di, di
    mov al, 05ah
    stosb                          ; selected graphics C-VID write path
    mov al, [es:0]
    cmp al, 05ah                   ; selected graphics C-VID read path
    jne failed

    mov ax, 0003h                  ; restore the public text Console mode
    int 10h
    mov dx, completion
    mov ah, 09h
    int 21h
    mov ax, 4c00h
    int 21h

failed:
    mov ax, 0003h
    int 10h
    mov dx, failure
    mov ah, 09h
    int 21h
    mov ax, 4c01h
    int 21h

completion db 'S23_GRAPHICS_VRAM_OK$'
failure    db 'S23_GRAPHICS_VRAM_FAIL$'
