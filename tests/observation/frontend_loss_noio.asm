; Authored test only: prove guest file work after root-frontend loss.
bits 16
org 100h
    mov dx,ready
    call create
wait_release:
    mov dx,release
    mov ax,3d00h
    int 21h
    jc wait_release
    mov bx,ax
    mov ah,3eh
    int 21h
    mov dx,done
    call create
    mov ax,4c07h
    int 21h
create:
    xor cx,cx
    mov ah,3ch
    int 21h
    jc failed
    mov bx,ax
    mov ah,3eh
    int 21h
    jc failed
    ret
failed:
    mov ax,4c63h
    int 21h
ready db 'logs\NIOREADY',0
release db 'logs\NIOGO',0
done db 'logs\NIODONE',0
