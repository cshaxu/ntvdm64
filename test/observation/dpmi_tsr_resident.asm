; Independent read-only witness for the preceding probe's resident DOS block.
bits 16
org 100h
%ifdef TSR_PM_RETENTION
    mov sp, stack_top
    mov bx, (image_end-$$+100h+15)/16
    mov ah, 4Ah
    int 21h
    jc failed
%endif
    mov dx, record_name
    mov ax, 3D00h
    int 21h
    jc failed
    mov bx, ax
    mov dx, record
    mov cx, record_end-record
    mov ah, 3Fh
    int 21h
    jc failed
    cmp ax, record_end-record
    jne failed
    mov ah, 3Eh
    int 21h
    jc failed
    mov ax, [record]
    dec ax
    mov es, ax
    mov al, [es:0]
    cmp al, 'M'
    je header_ok
    cmp al, 'Z'
    jne failed
header_ok:
    mov ax, [record]
    cmp [es:1], ax
    jne failed
    mov ax, [record+2]
    cmp [es:3], ax
    jne failed
    mov es, [record]
    mov di, [record+4]
    mov si, sentinel
    mov cx, sentinel_end-sentinel
    cld
    repe cmpsb
    jne failed
%ifdef TSR_PM_RETENTION
    mov ax, 1687h
    int 2Fh
    or ax, ax
    jnz failed
    mov [entry], di
    mov [entry+2], es
    or si, si
    jz enter_pm
    mov bx, si
    mov ah, 48h
    int 21h
    jc failed
    mov es, ax
enter_pm:
    xor ax, ax
    call far [entry]
    jc failed
    mov bx, [record+6]
    mov ax, 0006h
    int 31h
    jc failed
    cmp dx, [record+8]
    jne failed
    cmp cx, [record+10]
    jne failed
    lsl eax, bx
    jnz failed
    cmp eax, 4095
    jne failed
    mov es, bx
    cmp dword [es:0], 38525354h
    jne failed
    mov dx, pm_success
    mov ah, 09h
    int 21h
%endif
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
record_name db 'tests\D38TSR.DAT',0
record times 6 db 0
%ifdef TSR_PM_RETENTION
    times 10 db 0
%endif
record_end:
%ifdef TSR_PM_RETENTION
entry dd 0
pm_success db 'S38_TSR_PM_SELECTOR_MEMORY_RETAINED_OK',13,10,'$'
%endif
sentinel db 'S38-TSR-RESIDENT'
sentinel_end:
success db 'S38_TSR_MCB_OWNER_SIZE_CONTENT_OK',13,10,'$'
failure db 'S38_FAIL_TSR_RESIDENCY',13,10,'$'
%ifdef TSR_PM_RETENTION
    times 1024 db 0
stack_top:
image_end:
%endif
