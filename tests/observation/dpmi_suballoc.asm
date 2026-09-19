; Independent S36 probe; original DOSX and guest media are never modified.
; Register contracts: original dpmi/dxfunc.asm and dxint31.asm.
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
    mov [entry + 2], es
    mov bx, si
    test bx, bx
    jz enter_pm
    mov ah, 48h
    int 21h
    jc failed
    mov es, ax
enter_pm:
    mov dx, entering
    mov ah, 09h
    int 21h
    xor ax, ax
    call far [entry]
    jc failed
    mov [saved_es], es
    mov dx, entered
    mov ah, 09h
    int 21h
    xor bx, bx
    mov cx, 4096
    mov ax, 0501h
    int 31h
    jc failed
    mov [handle], di
    mov [handle + 2], si
    mov [linear], cx
    mov [linear + 2], bx
%ifdef STRESS
    xor bx, bx
    mov cx, 4096
    mov ax, 0501h
    int 31h
    jc failed
    mov [guard], di
    mov [guard + 2], si
    shl ebx, 16
    mov bx, cx
    mov eax, [linear]
    mov [old_linear], eax
    add eax, 4096
    cmp eax, ebx
    jne failed
%endif
    mov dx, allocated
    mov ah, 09h
    int 21h
    xor ax, ax
    mov cx, 1
    int 31h
    jc failed
    mov [selector], ax
    call bind_memory
    jc failed
    mov ax, [selector]
    mov es, ax
    mov dword [es:0], 36504D44h
    mov dword [es:4092], 214B4F36h
%ifdef STRESS
    ; Larger than this product's admitted pool: failure must retain the block.
    mov byte [failure_stage], '1'
    mov si, [handle + 2]
    mov di, [handle]
    mov bx, 0200h
    xor cx, cx
    mov ax, 0503h
    int 31h
    jnc failed
    mov byte [failure_stage], '2'
    cmp dword [es:0], 36504D44h
    jne failed
    cmp dword [es:4092], 214B4F36h
    jne failed
%endif
    mov si, [handle + 2]
    mov di, [handle]
    xor bx, bx
    mov cx, 8192
%ifdef STRESS
    mov byte [failure_stage], '3'
%endif
    mov ax, 0503h
    int 31h
    jc failed
    mov [handle], di
    mov [handle + 2], si
    mov [linear], cx
    mov [linear + 2], bx
%ifdef STRESS
    mov byte [failure_stage], '4'
    mov eax, [linear]
    cmp eax, [old_linear]
    je failed
    mov byte [failure_stage], '5'
%endif
    call bind_memory
    jc failed
    mov ax, [selector]
    mov es, ax
    cmp dword [es:0], 36504D44h
    jne failed
    cmp dword [es:4092], 214B4F36h
    jne failed
%ifdef RESTORE_ES
    mov ax, [saved_es]
%else
    xor ax, ax
%endif
    mov es, ax
    mov bx, [selector]
    mov ax, 0001h
    int 31h
    jc failed
    mov si, [handle + 2]
    mov di, [handle]
    mov ax, 0502h
    int 31h
    jc failed
%ifdef STRESS
    mov si, [guard + 2]
    mov di, [guard]
    mov ax, 0502h
    int 31h
    jc failed
    mov dx, stress_success
    mov ah, 09h
    int 21h
%endif
    mov dx, success
    mov ah, 09h
    int 21h
    mov ax, 4C00h
    int 21h

bind_memory:
    mov bx, [selector]
    mov cx, [linear + 2]
    mov dx, [linear]
    mov ax, 0007h
    int 31h
    jc .done
    mov bx, [selector]
    xor cx, cx
    mov dx, 8191
    mov ax, 0008h
    int 31h
.done:
    ret

failed:
    mov dx, failure
    mov ah, 09h
    int 21h
    mov ax, 4C01h
    int 21h

entry dw 0, 0
handle dd 0
linear dd 0
selector dw 0
saved_es dw 0
%ifdef STRESS
guard dd 0
old_linear dd 0
stress_success db 'S36_DPMI_FORCED_MOVE_FAILED_GROW_DATA_FREE_OK',13,10,'$'
%endif
entering db 'S36_DPMI_ENTERING',13,10,'$'
entered db 'S36_DPMI_ENTERED',13,10,'$'
allocated db 'S36_DPMI_ALLOCATED',13,10,'$'
success db 'S36_DPMI_ALLOC_REALLOC_DATA_FREE_OK',13,10,'$'
failure db 'S36_DPMI_FAIL stage='
failure_stage db '0',13,10,'$'
times 4096 db 0
stack_top:
image_end:
