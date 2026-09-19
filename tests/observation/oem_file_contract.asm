; Disposable DOS probe. CP437 9Ch is pound sign, with no case-fold ambiguity.
bits 16
org 100h
start:
    mov dx, directory
    mov ah, 39h
    int 21h
    jc failed
    mov dx, first_name
    xor cx, cx
    mov ah, 5Bh                 ; create NEW, never overwrite a test input
    int 21h
    jc failed
    mov bx, ax
    mov dx, payload
    mov cx, payload_end-payload
    mov ah, 40h
    int 21h
    jc failed
    cmp ax, payload_end-payload
    jne failed
    mov ah, 3Eh
    int 21h
    jc failed
    push ds
    pop es
    mov dx, first_name
    mov di, final_name
    mov ah, 56h
    int 21h
    jc failed
    mov dx, first_name
    mov ax, 3D00h
    int 21h
    jnc failed
    cmp ax, 2
    jne failed
    mov dx, final_name
    mov ax, 4301h
    mov cx, 1
    int 21h
    jc failed
    mov ax, 4300h
    int 21h
    jc failed
    test cx, 1
    jz failed
    mov ax, 4301h
    xor cx, cx
    int 21h
    jc failed
    mov ax, 3D00h
    int 21h
    jc failed
    mov bx, ax
    mov dx, buffer
    mov cx, payload_end-payload
    mov ah, 3Fh
    int 21h
    jc failed
    cmp ax, payload_end-payload
    jne failed
    mov ah, 3Eh
    int 21h
    jc failed
    mov si, buffer
    mov di, payload
    mov cx, payload_end-payload
    cld
    repe cmpsb
    jne failed
    mov dx, success
    mov ah, 09h
    int 21h
    mov ax, 4C00h
    int 21h
failed:
    mov dx, failure
    mov ah, 09h
    int 21h
    mov ax, 4C01h
    int 21h
directory db 'tests\O37G\D',09Ch,0
first_name db 'tests\O37G\D',09Ch,'\A',09Ch,'.DAT',0
final_name db 'tests\O37G\D',09Ch,'\B',09Ch,'.DAT',0
payload db 'S37-OEM-GUEST-BYTES',13,10
payload_end:
buffer times 32 db 0
success db 'S37_OEM_GUEST_CREATE_RENAME_ATTR_READ_OK',13,10,'$'
failure db 'S37_OEM_GUEST_FAIL',13,10,'$'
