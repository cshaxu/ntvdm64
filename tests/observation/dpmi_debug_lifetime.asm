; Independent observation of original DOSX task-exit debug-handle cleanup.
; Use an untouched address: never alter original guest memory or provoke a
; stale data breakpoint in the returning COMMAND's actual address range.
bits 16
org 100h
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
    mov ax, 0400h
    int 31h
    jc failed
    cmp ax, 005Ah             ; Original DOSX reports DPMI 0.90.
    jne failed
%ifdef INVALID_REQUEST
    ; A rejected length must not consume an otherwise free debug slot.
    mov ax, 0B00h
    mov bx, 7FF0h
    xor cx, cx
    mov dx, 0103h             ; Three-byte watchpoints are invalid.
    int 31h
    jnc failed
%endif
    mov ax, 0B00h
    mov bx, 7FF0h
    xor cx, cx
    mov dx, 0101h
    int 31h
    jc failed
    cmp bx, 3
    ja failed
    mov [saved_handle], bx
    add bl, '0'
    mov [handle], bl
    mov dx, message
    mov ah, 09h
    int 21h
%ifdef INVALID_REQUEST
    cmp word [saved_handle], 0
    jne failed
%endif
%ifdef EXPLICIT_RELEASE
    mov bx, [saved_handle]
    mov ax, 0B01h
    int 31h
    jc failed
%endif
    ; Deliberately no 0B01: task exit is the contract under observation.
    mov ax, 4C00h
    int 21h
failed:
    mov dx, failure
    mov ah, 09h
    int 21h
    mov ax, 4C01h
    int 21h
entry dd 0
saved_handle dw 0
message db 'S38_DEBUG_EXIT_HANDLE='
handle db '?',13,10,'$'
failure db 'S38_DEBUG_EXIT_ALLOCATION_FAILED',13,10,'$'
times 4096 db 0
stack_top:
image_end:
