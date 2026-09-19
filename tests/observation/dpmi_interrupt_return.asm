; Independent DOS test, not a replacement or patch for original guest media.
; Contracts: dpmi/486/dxint31.asm and dpmi32/dpmiint.c.
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
    mov [entry+2], es
    mov bx, si
    test bx, bx
    jz enter_pm
    mov ah, 48h
    int 21h
    jc failed
    mov es, ax
enter_pm:
    xor ax, ax
    call far [entry]
    jc failed
    mov byte [stage], '1'
    mov ax, 0204h
    mov bl, 60h
    int 31h
    jc failed
    mov [old_int], dx
    mov [old_int+2], cx
    mov ax, 0205h
    mov bl, 60h
    mov cx, cs
    mov dx, interrupt_handler
    int 31h
    jc failed
    mov [saved_sp], sp
    mov ax, 1234h
    stc
    int 60h
    jnc failed
    cmp ax, 1234h
    jne failed
    cmp sp, [saved_sp]
    jne failed
    cmp word [int_count], 1
    jne failed
    mov dx, [old_int]
    mov cx, [old_int+2]
    mov ax, 0205h
    mov bl, 60h
    int 31h
    jc failed
    mov dx, int_ok
    mov ah, 09h
    int 21h
    mov byte [stage], '2'
    mov ax, 0202h
    xor bx, bx
    int 31h
    jc failed
    mov [old_fault], dx
    mov [old_fault+2], cx
    mov ax, 0203h
    xor bx, bx
    mov cx, cs
    mov dx, fault_handler
    int 31h
    jc failed
    mov [saved_sp], sp
    xor dx, dx
    xor bx, bx
    mov ax, 4321h
    stc
fault_instruction:
    div bx
fault_resume:
    jnc failed
    cmp ax, 4321h
    jne failed
    cmp sp, [saved_sp]
    jne failed
    cmp word [fault_count], 1
    jne failed
    mov ax, 0203h
    xor bx, bx
    mov dx, [old_fault]
    mov cx, [old_fault+2]
    int 31h
    jc failed
    mov byte [stage], '3'
    mov ax, 0202h
    mov bl, 11h                 ; Original DOSX rejects exception > 10h.
    int 31h
    jnc failed
    mov dx, fault_ok
    mov ah, 09h
    int 21h
    mov ax, 4C00h
    int 21h
interrupt_handler:
    inc word [int_count]
    iret
fault_handler:
    push bp
    mov bp, sp
    ; far return, error word, IP, CS, FLAGS, SP, SS (16-bit client).
    add word [ss:bp+8], fault_resume-fault_instruction
    inc word [fault_count]
    pop bp
    retf
failed:
    mov dx, fail_message
    mov ah, 09h
    int 21h
    mov ax, 4C01h
    int 21h
entry dd 0
old_int dd 0
old_fault dd 0
saved_sp dw 0
int_count dw 0
fault_count dw 0
int_ok db 'S38_INT16_RETURN_OK',13,10,'$'
fault_ok db 'S38_FAULT16_RETURN_NEGATIVE_OK',13,10,'$'
fail_message db 'S38_FAIL_STAGE='
stage db '0',13,10,'$'
times 4096 db 0
stack_top:
image_end:
