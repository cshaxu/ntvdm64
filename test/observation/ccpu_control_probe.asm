; Independently authored real CPU40 guest workload. Original guest files stay unchanged.
bits 16
org 100h
jmp start

%macro FAULT_SITE 2
    mov word [fault_ip],%%fault
    mov word [resume_ip],%%resume
%%fault:
    %1 %2
%%resume:
%endmacro

%macro DIV_FAULT 4
    mov eax,%2
    mov edx,%3
    mov ebx,%4
    mov byte [seen_vector],0xff
    mov word [fault_ip],%%fault
    mov word [resume_ip],%%resume
%%fault:
    div %1
%%resume:
    cmp byte [seen_vector],0
    jne failed
    cmp eax,%2
    jne failed
    cmp edx,%3
    jne failed
    cmp ebx,%4
    jne failed
    cmp byte [fault_error],0
    jne failed
%endmacro

start:
    push cs
    pop ds
    pushfd
    pop dword [original_flags]
    mov eax,cr0
    mov [original_cr0],eax
    mov ax,3500h
    int 21h
    mov [old0],bx
    mov [old0+2],es
    mov ax,3506h
    int 21h
    mov [old6],bx
    mov [old6+2],es
    mov ax,3507h
    int 21h
    mov [old7],bx
    mov [old7+2],es
    mov dx,handler0
    mov ax,2500h
    int 21h
    mov dx,handler6
    mov ax,2506h
    int 21h
    mov dx,handler7
    mov ax,2507h
    int 21h
    mov byte [installed],1
    mov byte [phase],'A'
    xor esi,esi
.flags:
    mov eax,esi
    imul eax,0x49ed
    and eax,0x0cd5
    mov edx,[original_flags]
    and edx,~0x0ed5
    or eax,edx
    or eax,0x200
    mov [expected_flags],eax
    mov eax,0x12345678
    mov ebx,0x87654321
    push dword [expected_flags]
    popfd
    nop
    pushfd
    pop ecx
    cmp eax,0x12345678
    jne failed
    cmp ebx,0x87654321
    jne failed
    xor ecx,[expected_flags]
%ifdef CPU_PROBE_NEGATIVE_ORACLE
    xor ecx,1
%endif
    test ecx,0x0ed5
    jnz failed
    ; CLI must change IF only, after the decoder's permission check.
    push dword [expected_flags]
    popfd
    cli
    pushfd
    pop ecx
    xor ecx,[expected_flags]
    cmp ecx,0x200
    jne failed
    push dword [original_flags]
    popfd
    inc esi
    cmp esi,32
    jb .flags

    mov byte [phase],'B'
    cli
    xor esi,esi
.clts:
    mov eax,[original_cr0]
    and eax,~0x0a
    mov ebx,esi
    and ebx,1
    shl ebx,1
    or eax,ebx
    mov ebx,esi
    and ebx,2
    shl ebx,2
    or eax,ebx
    mov cr0,eax
    and eax,~8
    mov ebx,eax
    clts
    mov eax,cr0
    cmp eax,ebx
    jne failed
    inc esi
    cmp esi,4
    jb .clts
    mov eax,[original_cr0]
    mov cr0,eax

    mov byte [phase],'C'
    xor esi,esi
.wait:
    mov eax,[original_cr0]
    and eax,~0x0a
    mov ebx,esi
    and ebx,1
    shl ebx,1
    or eax,ebx
    mov ebx,esi
    and ebx,2
    shl ebx,2
    or eax,ebx
    mov cr0,eax
    mov byte [seen_vector],0xff
    mov word [fault_ip],.wait_instruction
    mov word [resume_ip],.wait_return
.wait_instruction:
    db 0x9b
.wait_return:
    mov eax,[original_cr0]
    mov cr0,eax
    cmp byte [fault_error],0
    jne failed
    cmp esi,3
    je .wait_fault
    cmp byte [seen_vector],0xff
    jne failed
    jmp .wait_next
.wait_fault:
    cmp byte [seen_vector],7
    jne failed
.wait_next:
    inc esi
    cmp esi,4
    jb .wait

    mov byte [phase],'D'
    ; All IDTR changes stay in a short IF-clear interval and restore the
    ; captured IDTR before DOS calls. No original firmware/table is modified.
    o32 sidt [original_idtr]
    mov byte [idt_saved],1
    xor esi,esi
.idt:
    mov word [input_idtr],0x5a5a
    test esi,1
    jz .limit_ready
    mov word [input_idtr],0xa5a5
.limit_ready:
    mov edi,esi
    shr edi,1
    mov eax,[bases+edi*4]
    mov [input_idtr+2],eax
    o32 lidt [input_idtr]
    o32 sidt [actual_idtr]
    o32 lidt [original_idtr]
    mov ax,[input_idtr]
    cmp ax,[actual_idtr]
    jne failed
    mov eax,[input_idtr+2]
    cmp eax,[actual_idtr+2]
    jne failed
    o16 lidt [input_idtr]
    o16 sidt [actual_idtr]
    o32 lidt [original_idtr]
    mov ax,[input_idtr]
    cmp ax,[actual_idtr]
    jne failed
    mov eax,[input_idtr+2]
    and eax,0x00ffffff
    cmp eax,[actual_idtr+2]
    jne failed
    inc esi
    cmp esi,8
    jb .idt
    mov byte [idt_saved],0

    mov byte [phase],'E'
    DIV_FAULT bl,0x12340012,0x56789abc,0
    DIV_FAULT bx,0x12340012,0x56789abc,0
    DIV_FAULT ebx,0x12340012,0x56789abc,0
    DIV_FAULT bl,0x12340100,0x56789abc,1
    DIV_FAULT bx,0x12340000,0x56780001,1
    DIV_FAULT ebx,0,1,1

    mov byte [phase],'F'
    mov ecx,0x00010000
    jcxz .cx_zero
    jmp failed
.cx_zero:
    jecxz failed
    mov ecx,0
    jecxz .ecx_zero
    jmp failed
.ecx_zero:
    mov ecx,1
    jcxz failed

    mov byte [phase],'G'
    mov eax,0x12345678
    mov byte [seen_vector],0xff
    FAULT_SITE str,ax
    cmp byte [seen_vector],6
    jne failed
    cmp byte [fault_error],0
    jne failed
    cmp eax,0x12345678
    jne failed
    ; Specific original reserved routes; this is not a universal #UD rule.
    db 0x0f,0x07,0x0f,0x10,0x0f,0x11,0x0f,0x12,0x0f,0x13
    cmp dword [faults],8 ; WAIT + six DIV + real-mode STR
    jne failed
    call restore
    mov dx,pass_message
    mov ah,9
    int 21h
    mov ax,4c00h
    int 21h

failed:
    call restore
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

restore:
    cli
    mov eax,[original_cr0]
    mov cr0,eax
    cmp byte [idt_saved],0
    je .vectors
    o32 lidt [original_idtr]
.vectors:
    cmp byte [installed],0
    je .flags
    mov dx,[old0]
    mov ax,[old0+2]
    mov ds,ax
    mov ax,2500h
    int 21h
    push cs
    pop ds
    mov dx,[old6]
    mov ax,[old6+2]
    mov ds,ax
    mov ax,2506h
    int 21h
    push cs
    pop ds
    mov dx,[old7]
    mov ax,[old7+2]
    mov ds,ax
    mov ax,2507h
    int 21h
    push cs
    pop ds
.flags:
    push dword [original_flags]
    popfd
    ret

handler0:
    mov byte [cs:seen_vector],0
    jmp handler
handler6:
    mov byte [cs:seen_vector],6
    jmp handler
handler7:
    mov byte [cs:seen_vector],7
handler:
    push bp
    mov bp,sp
    push ax
    inc dword [cs:faults]
    mov ax,[ss:bp+2]
    cmp ax,[cs:fault_ip]
    jne .bad_frame
    mov ax,cs
    cmp ax,[ss:bp+4]
    je .ip_ok
.bad_frame:
    mov byte [cs:fault_error],1
.ip_ok:
    mov ax,[cs:resume_ip]
    mov [ss:bp+2],ax
    pop ax
    pop bp
    iret

original_flags dd 0
original_cr0 dd 0
expected_flags dd 0
faults dd 0
old0 dd 0
old6 dd 0
old7 dd 0
fault_ip dw 0
resume_ip dw 0
seen_vector db 0xff
fault_error db 0
installed db 0
idt_saved db 0
phase db 'A'
bases dd 0,0x00123456,0x12345678,0xffffffff
original_idtr times 6 db 0
input_idtr times 6 db 0
actual_idtr times 6 db 0
pass_message db 'CCPU CONTROL PASS',13,10,'NOP32 CLI32 CLTS4 WAIT4 IDT16',13,10,'DIVFAULT6 JCXZ4 STRFAULT1 RSRVD5',13,10,'$'
fail_message db 'CCPU CONTROL FAIL phase=','$'
newline db 13,10,'$'
