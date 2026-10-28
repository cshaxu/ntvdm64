; Independent CPU40 BIOS dispatch witness. No original guest code is changed.
bits 16
org 100h
jmp start

%macro PREPARE 0
    mov bx,[index]
    and bx,3
    shl bx,1
    mov ax,[patterns+bx]
    mov [expected_flags],ax
    mov eax,0xa5a50000
    mov ax,[input_ax]
    mov ebx,0x13572468
    mov ecx,0x24683579
    mov edx,0x369c48ad
    mov esi,0x55aa33cc
    mov edi,0xaa55cc33
    mov ebp,0x67891234
    push word [expected_flags]
    popf
    mov [before_sp],sp
%endmacro

%macro ASSERT_STATE 0
    pushf
    pop word [actual_flags]
    cmp ax,[expected_ax]
    jne failed
    shr eax,16
    cmp ax,0xa5a5
    jne failed
    cmp ebx,0x13572468
    jne failed
    cmp ecx,0x24683579
    jne failed
    cmp edx,0x369c48ad
    jne failed
    cmp esi,0x55aa33cc
    jne failed
    cmp edi,0xaa55cc33
    jne failed
    cmp ebp,0x67891234
    jne failed
    cmp sp,[before_sp]
    jne failed
    mov ax,ds
    cmp ax,[guest_segment]
    jne failed
    mov ax,es
    cmp ax,[guest_segment]
    jne failed
    mov ax,[actual_flags]
    xor ax,[expected_flags]
    and ax,0x0cd5
    jnz failed
    inc word [asserted]
%endmacro

start:
    push cs
    pop ds
    push cs
    pop es
    mov [guest_segment],ds
    mov byte [phase],'C'
.kind:
    mov word [index],0
.control:
    mov ax,[index]
    mov word [expected_ax],1
    mov ah,1                       ; GENERIC
    cmp byte [kind],0
    je .ready
    mov ah,4                       ; NT HOST_TYPE
    cmp byte [kind],1
    je .ready
    mov ah,2                       ; wrong host
    mov word [expected_ax],2
.ready:
    mov [input_ax],ax
%ifdef CPU_PROBE_NEGATIVE_ORACLE
    mov word [expected_ax],0        ; deliberate mismatch, first case fails
%endif
    PREPARE
    db 0xc4,0xc4,0xff
    ASSERT_STATE
    inc word [index]
    cmp word [index],256
    jb .control
    inc byte [kind]
    cmp byte [kind],3
    jb .kind

    mov byte [phase],'I'
    mov word [index],0
    mov word [input_ax],0xbeef
    mov word [expected_ax],0xbeef
.illegal:
    PREPARE
    db 0xc4,0xc4,0xf0
    ASSERT_STATE
    PREPARE
    db 0xc4,0xc4,0xf7
    ASSERT_STATE
    inc word [index]
    cmp word [index],32
    jb .illegal

    mov byte [phase],'E'
    mov word [index],0
    mov word [expected_ax],0x845a
.ems:
    mov bx,[index]
    mov ah,[invalid_ems+bx]
    mov al,0x5a
    mov [input_ax],ax
    PREPARE
    db 0xc4,0xc4,0x67
    ASSERT_STATE
    inc word [index]
    cmp word [index],4
    jb .ems
    mov byte [phase],'N'
    cmp word [asserted],836
    jne failed
    cld
    mov dx,pass_message
    mov ah,9
    int 21h
    mov ax,4c00h
    int 21h
failed:
    cld
    mov dx,fail_message
    mov ah,9
    int 21h
    mov dl,[phase]
    mov ah,2
    int 21h
    mov dx,newline
    mov ah,9
    int 21h
    mov ax,4c01h
    int 21h

index dw 0
kind db 0
phase db 'A'
asserted dw 0
input_ax dw 0
expected_ax dw 0
guest_segment dw 0
before_sp dw 0
expected_flags dw 0
actual_flags dw 0
patterns dw 0x0202,0x02d7,0x0a02,0x0603
invalid_ems db 0,0x3f,0x60,0xff
pass_message db 'BIOS DISPATCH PASS cases=836',13,10,'CONTROL768 ILLEGAL64 EMSBAD4',13,10,'$'
fail_message db 'BIOS DISPATCH FAIL phase=','$'
newline db 13,10,'$'
