; The former Build-T420S44CommandFixture byte array, unchanged in behavior.
bits 16
org 100h
    mov dx,message
    mov ah,09h
    int 21h
    mov ax,4c07h
    int 21h
message db 'S10_GUEST_SEVEN$'
