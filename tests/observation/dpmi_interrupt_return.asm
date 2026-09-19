; Independent DOS test, not a replacement or patch for original guest media.
; Contracts: dpmi/486/dxint31.asm and dpmi32/dpmiint.c.
bits 16
org 100h
%ifdef CODE32
%ifndef CLIENT32
%error CODE32 requires CLIENT32
%endif
%define failed failed16
%endif
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
%ifdef CLIENT32
    mov ax, 1                   ; Select 32-bit DPMI return-frame contract.
%else
    xor ax, ax
%endif
    call far [entry]
    jc failed
%ifdef CODE32
    ; Publish a separate code selector using the original descriptor API.
    ; Original DOSX is unchanged; only this probe's own descriptor is edited.
    push ds
    pop es
    mov bx, cs
    mov edi, descriptor
    mov ax, 000Bh
    int 31h
    jc failed
    xor ax, ax
    mov cx, 1
    int 31h
    jc failed
    mov [code_entry+4], ax
    mov bx, ax
    or byte [descriptor+6], 40h
    mov edi, descriptor
    mov ax, 000Ch
    int 31h
    jc failed
    movzx esp, sp
    jmp dword far [code_entry]
failed16:
    mov dx, fail_message
    mov ah, 09h
    int 21h
    mov ax, 4C01h
    int 21h
%undef failed
bits 32
code32_start:
%endif
    mov byte [stage], '1'
    mov ax, 0204h
    mov bl, 60h
    int 31h
    jc failed
    mov [old_int], edx
    mov [old_int+4], cx
    mov ax, 0205h
    mov bl, 60h
    mov cx, cs
    mov edx, interrupt_handler
    int 31h
    jc failed
    mov [saved_sp], sp
    mov eax, 56781234h
    stc
    int 60h
    jnc failed
    cmp eax, 56781234h
    jne failed
    cmp sp, [saved_sp]
    jne failed
    cmp word [int_count], 1
    jne failed
    mov edx, [old_int]
    mov cx, [old_int+4]
    mov ax, 0205h
    mov bl, 60h
    int 31h
    jc failed
    mov edx, int_ok
    mov ah, 09h
    int 21h
    mov byte [stage], '2'
    mov ax, 0202h
    xor bx, bx
    int 31h
    jc failed
    mov [old_fault], edx
    mov [old_fault+4], cx
    mov ax, 0203h
    xor bx, bx
    mov cx, cs
    mov edx, fault_handler
    int 31h
    jc failed
    mov [saved_sp], sp
    xor dx, dx
    xor bx, bx
    mov eax, 87654321h
    stc
fault_instruction:
    div bx
fault_resume:
    jnc failed
    cmp eax, 87654321h
    jne failed
    cmp sp, [saved_sp]
    jne failed
    cmp word [fault_count], 1
    jne failed
    mov ax, 0203h
    xor bx, bx
    mov edx, [old_fault]
    mov cx, [old_fault+4]
    int 31h
    jc failed
    mov byte [stage], '3'
    mov ax, 0202h
    mov bl, 11h                 ; Original DOSX rejects exception > 10h.
    int 31h
    jnc failed
    mov edx, fault_ok
    mov ah, 09h
    int 21h
    mov ax, 4C00h
    int 21h
interrupt_handler:
    inc word [int_count]
%ifdef CLIENT32
    iretd
%else
    iret
%endif
fault_handler:
%ifdef CODE32
    push ebp
    mov ebp, esp
    add dword [ss:ebp+16], fault_resume-fault_instruction
%else
    push bp
    mov bp, sp
%ifdef CLIENT32
    ; 32-bit far return (8), error (4), EIP; plus saved BP (2).
    add dword [ss:bp+14], fault_resume-fault_instruction
%else
    ; far return, error word, IP, CS, FLAGS, SP, SS (16-bit client).
    add word [ss:bp+8], fault_resume-fault_instruction
%endif
%endif
    inc word [fault_count]
%ifdef CODE32
    pop ebp
%else
    pop bp
%endif
%ifdef CLIENT32
    o32 retf
%else
    retf
%endif
failed:
    mov edx, fail_message
    mov ah, 09h
    int 21h
    mov ax, 4C01h
    int 21h
entry dd 0
%ifdef CODE32
code_entry dd code32_start
    dw 0
descriptor times 8 db 0
%endif
old_int dd 0
    dw 0
old_fault dd 0
    dw 0
saved_sp dw 0
int_count dw 0
fault_count dw 0
%ifdef CLIENT32
int_ok db 'S38_INT32_RETURN_OK',13,10,'$'
fault_ok db 'S38_FAULT32_RETURN_NEGATIVE_OK',13,10,'$'
%else
int_ok db 'S38_INT16_RETURN_OK',13,10,'$'
fault_ok db 'S38_FAULT16_RETURN_NEGATIVE_OK',13,10,'$'
%endif
fail_message db 'S38_FAIL_STAGE='
stage db '0',13,10,'$'
times 4096 db 0
stack_top:
image_end:
