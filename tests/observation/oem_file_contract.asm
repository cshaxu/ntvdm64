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
    ; Exercise original directory, deletion and disk-info services in guest.
    mov si, saved_directory+1
    xor dl, dl
    mov ah, 47h
    int 21h
    jc failed
    mov dx, directory
    mov ah, 3Bh
    int 21h
    jc failed
    ; The original GetCDSFromDrv uses CMDSVC 54:04 with AL drive, DS:SI.
    ; Explicitly query after DEM's chdir has published its =X: environment.
    mov ah, 19h
    int 21h
    mov si, environment_directory
    db 0c4h, 0c4h, 54h, 04h
    jc failed
    cmp byte [environment_directory+1], ':'
    jne failed
    cmp byte [environment_directory+2], '\'
    jne failed
    mov si, environment_directory
    xor cx, cx
environment_length:
    cmp byte [si], 0
    je environment_ready
    inc si
    inc cx
    cmp cx, 259
    jae failed
    jmp environment_length
environment_ready:
    push cx
    mov dx, environment_prefix
    mov ah, 09h
    int 21h
    pop cx
    mov dx, environment_directory
    mov bx, 1
    mov ah, 40h
    int 21h
    jc failed
    cmp ax, cx
    jne failed
    mov dx, environment_success
    mov ah, 09h
    int 21h
    mov dx, scratch_name
    xor cx, cx
    mov ah, 5Bh
    int 21h
    jc failed
    mov bx, ax
    mov ah, 3Eh
    int 21h
    jc failed
    mov dx, scratch_name
    mov ah, 41h
    int 21h
    jc failed
    mov ax, 3D00h
    int 21h
    jnc failed
    cmp ax, 2
    jne failed
    mov dx, scratch_directory
    mov ah, 39h
    int 21h
    jc failed
    mov ah, 3Ah
    int 21h
    jc failed
    mov dx, fcb_first
    call create_empty
    mov dx, fcb_second
    call create_empty
    mov dx, delete_fcb
    mov ah, 13h
    int 21h
    test al, al
    jnz failed
    mov dx, fcb_first
    mov ax, 3D00h
    int 21h
    jnc failed
    cmp ax, 2
    jne failed
    mov dx, fcb_second
    mov ax, 3D00h
    int 21h
    jnc failed
    cmp ax, 2
    jne failed
    mov dx, delete_fcb
    mov ah, 13h
    int 21h
    cmp al, 0FFh
    jne failed
    mov dx, computer_name
    mov ax, 5E00h
    int 21h
    jc failed
    test ch, ch
    jz failed
    cmp byte [computer_name+15], 0
    jne failed
    mov dx, computer_prefix
    mov ah, 09h
    int 21h
    mov bx, 1
    mov dx, computer_name
    mov cx, 15
    mov ah, 40h
    int 21h
    jc failed
    cmp ax, 15
    jne failed
    mov dx, fcb_success
    mov ah, 09h
    int 21h
    xor dl, dl
    mov ah, 36h
    int 21h
    cmp ax, 0FFFFh
    je failed
    test ax, ax
    jz failed
    test cx, cx
    jz failed
    test dx, dx
    jz failed
    cmp bx, dx
    ja failed
    mov ax, 4408h
    xor bx, bx
    int 21h
    jc failed
    cmp ax, 1
    ja failed
    mov dx, saved_directory
    mov ah, 3Bh
    int 21h
    jc failed
    mov si, restored_directory
    xor dl, dl
    mov ah, 47h
    int 21h
    jc failed
    mov si, restored_directory
    mov di, saved_directory+1
    mov cx, 64
    cld
    repe cmpsb
    jne failed
    mov dx, directory_success
    mov ah, 09h
    int 21h
    mov dx, success
    mov ah, 09h
    int 21h
    mov ax, 4C00h
    int 21h
create_empty:
    xor cx, cx
    mov ah, 5Bh
    int 21h
    jc failed
    mov bx, ax
    mov ah, 3Eh
    int 21h
    jc failed
    ret
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
saved_directory db '\'
    times 64 db 0
restored_directory times 64 db 0
scratch_name db 'C',09Ch,'.DAT',0
scratch_directory db 'E',09Ch,0
fcb_first db 'F',09Ch,'1.DAT',0
fcb_second db 'F',09Ch,'2.DAT',0
delete_fcb db 0,'F',09Ch,'??????','DAT'
    times 25 db 0
computer_name times 16 db 0
computer_prefix db 'S37_HOST=','$'
environment_directory times 260 db 0
environment_prefix db 'S37_ENV_DIR=','$'
environment_success db 13,10,'S37_OEM_GUEST_COMMAND_ENV_OK',13,10,'$'
fcb_success db 13,10,'S37_OEM_GUEST_FCB_COMPUTER_OK',13,10,'$'
directory_success db 'S37_OEM_GUEST_DIRECTORY_DELETE_DISK_OK',13,10,'$'
success db 'S37_OEM_GUEST_CREATE_RENAME_ATTR_READ_OK',13,10,'$'
failure db 'S37_OEM_GUEST_FAIL',13,10,'$'
