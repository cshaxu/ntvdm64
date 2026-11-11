; S45 real-worker check of VCD_PM_Get_Port_Array.
; EXPECT_PORT_MASK is generated from the public host DOS-device namespace;
; neither this probe nor the product reads or changes the Windows Registry.
bits 16
org 100h

%ifndef EXPECT_PORT_MASK
%error EXPECT_PORT_MASK must be supplied by the test runner
%endif

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
    jz .enter
    mov ah, 48h
    int 21h
    jc failed
    mov es, ax
.enter:
    xor ax, ax
    call far [entry]
    jc failed
    mov ax, 1684h
    mov bx, 0Eh
    xor di, di
    int 2Fh
    mov ax, es
    test ax, ax
    jz failed
    mov [service], di
    mov [service+2], ax
    mov dx, 1
    call far [service]
    cmp ax, EXPECT_PORT_MASK
    jne failed
    mov ax, 4C00h
    int 21h
failed:
    mov ax, 4C01h
    int 21h

entry dd 0
service dd 0
times 4096 db 0
stack_top:
image_end:
