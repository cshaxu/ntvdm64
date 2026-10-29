; Disposable NTVDM-only test input. Trigger the original BOP B1 dispatcher
; with an unrecognised control code, whose default branch performs no action.
; This checks the observer transport; it is not an original guest component.
bits 16
org 0x100
    mov eax,0xffff
    xor bx,bx
    xor cx,cx
    xor dx,dx
    db 0xc4,0xc4,0xb1
    mov ax,0x4c00
    int 0x21
