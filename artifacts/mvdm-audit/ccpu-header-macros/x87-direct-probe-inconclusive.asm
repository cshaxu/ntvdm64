; Independent CPU40 segment-boundary witness. Descriptors, gates and the
; complete 128-byte backing arena belong to this fresh DOS test process.
bits 16
org 100h
start:
    push cs
    pop ds
    mov [old_ss],ss
    mov [old_sp],sp
    mov sp,stack_top
    pushfd
    pop dword [old_flags]
    mov eax,cr0
    mov [old_cr0],eax
    test al,1
    jnz real_setup_failed
    mov dx,start_message
    mov ah,9
    int 21h
    cli
    o32 sgdt [old_gdtr]
    o32 sidt [old_idtr]
    mov [old_es],es
    mov [old_fs],fs
    mov [old_gs],gs
    mov ax,cs
    mov [return_pointer+2],ax
    movzx eax,ax
    shl eax,4
    mov [code_descriptor+2],ax
    mov [data_descriptor+2],ax
    mov edx,eax
    shr edx,16
    mov [code_descriptor+4],dl
    mov [data_descriptor+4],dl
    mov edx,eax
    add edx,gdt
    mov [gdtr+2],edx
    mov edx,eax
    add edx,idt
    mov [idtr+2],edx
    add eax,arena
    mov [narrow_descriptor+2],ax
    shr eax,16
    mov [narrow_descriptor+4],al
    mov word [resume_ip],pm_abort
    o32 lgdt [gdtr]
    o32 lidt [idtr]
    mov eax,[old_cr0]
    and eax,~0x0e
    or eax,1
    mov cr0,eax
    jmp 8:protected_start
protected_start:
    mov ax,16
    mov ds,ax
    mov es,ax
    mov ss,ax
    mov ax,24
    mov fs,ax
%ifdef CPU_X87_TRANSITION_CONTROL
    mov word [fault_ip],control_fault
    mov word [resume_ip],control_resume
    mov eax,0x12345678
control_fault:
    o32 mov eax,[fs:108]
control_resume:
    cmp eax,0x12345678
    jne pm_abort
    jmp leave_protected
%endif
    call fill
    fninit
    mov word [fault_ip],case1
    mov word [resume_ip],case1_done
case1:
    o32 fnsave [fs:0]
case1_done:
    cmp word [faults],0
    jne .next
    mov si,arena+108
    mov cx,20
    call untouched
    jc .next
    mov si,arena
    mov cx,108
    call written
    jc .next
    mov byte [wide_ok],1
.next:
    mov word [narrow_descriptor],93
    mov ax,24
    mov fs,ax
    call fill
    fninit
    mov word [fault_ip],case2
    mov word [resume_ip],case2_done
case2:
    o32 fnsave [fs:0]
case2_done:
    cmp word [faults],1
    jne .next
    mov si,arena
    mov cx,128
    call untouched
    jc .next
    mov byte [narrow32_ok],1
.next:
    call fill
    fninit
    mov word [fault_ip],case3
    mov word [resume_ip],case3_done
case3:
    o16 fnsave [fs:0]
case3_done:
    cmp word [faults],0
    jne pm_done
    mov si,arena+94
    mov cx,34
    call untouched
    jc pm_done
    mov si,arena
    mov cx,94
    call written
    jc pm_done
    mov byte [narrow16_ok],1
pm_done:
    jmp leave_protected
pm_abort:
    mov byte [bad_frame],1
leave_protected:
    mov eax,[old_cr0]
    mov cr0,eax
    jmp far [cs:return_pointer]
real_return:
    push cs
    pop ds
    mov ax,[old_ss]
    mov ss,ax
    mov sp,[old_sp]
    o32 lgdt [old_gdtr]
    o32 lidt [old_idtr]
    mov es,[old_es]
    mov fs,[old_fs]
    mov gs,[old_gs]
    push dword [old_flags]
    popfd
%ifdef CPU_X87_TRANSITION_CONTROL
    cmp word [faults],1
    jne real_failed
    cmp byte [bad_frame],0
    jne real_failed
    mov dx,transition_message
    mov ah,9
    int 21h
    mov ax,4c00h
    int 21h
%endif
    mov dx,wide_fail
    cmp byte [wide_ok],1
    jne .wide_report
    mov dx,wide_pass
.wide_report:
    mov ah,9
    int 21h
    mov dx,narrow32_fail
    cmp byte [narrow32_ok],1
    jne .n32_report
    mov dx,narrow32_pass
.n32_report:
    mov ah,9
    int 21h
    mov dx,narrow16_fail
    cmp byte [narrow16_ok],1
    jne .n16_report
    mov dx,narrow16_pass
.n16_report:
    mov ah,9
    int 21h
    cmp byte [bad_frame],0
    jne real_failed
    cmp byte [wide_ok],1
    jne real_failed
    cmp byte [narrow32_ok],1
    jne real_failed
    cmp byte [narrow16_ok],1
    jne real_failed
    mov ax,4c00h
    int 21h
real_setup_failed:
    mov dx,setup_message
    mov ah,9
    int 21h
real_failed:
    mov ax,4c01h
    int 21h
fill:
    cld
    mov di,arena
    mov cx,128
    mov al,0a5h
    rep stosb
    mov word [faults],0
    ret
untouched:
    cmp byte [si],0a5h
    jne .bad
    inc si
    loop untouched
    clc
    ret
.bad:
    stc
    ret
written:
    cmp byte [si],0a5h
    jne .yes
    inc si
    loop written
    stc
    ret
.yes:
    clc
    ret
gp_handler:
    push bp
    mov bp,sp
    push ax
    inc word [cs:faults]
    mov ax,[ss:bp+4]
    cmp ax,[cs:fault_ip]
    jne .bad
    mov ax,cs
    cmp ax,[ss:bp+6]
    jne .bad
    cmp word [ss:bp+2],0
    je .resume
.bad:
    mov byte [cs:bad_frame],1
    mov word [cs:resume_ip],pm_abort
.resume:
    mov ax,[cs:resume_ip]
    mov [ss:bp+4],ax
    pop ax
    pop bp
    add sp,2
    iret
old_cr0 dd 0
old_flags dd 0
old_ss dw 0
old_sp dw 0
old_es dw 0
old_fs dw 0
old_gs dw 0
old_gdtr times 6 db 0
old_idtr times 6 db 0
return_pointer dw real_return,0
faults dw 0
fault_ip dw 0
resume_ip dw 0
wide_ok db 0
narrow32_ok db 0
narrow16_ok db 0
bad_frame db 0
start_message db 'X87 START',13,10,'$'
wide_pass db 'X87 WIDE32 PASS',13,10,'$'
wide_fail db 'X87 WIDE32 FAIL',13,10,'$'
narrow32_pass db 'X87 NARROW32 PASS',13,10,'$'
narrow32_fail db 'X87 NARROW32 FAIL',13,10,'$'
narrow16_pass db 'X87 NARROW16 PASS',13,10,'$'
narrow16_fail db 'X87 NARROW16 FAIL',13,10,'$'
setup_message db 'X87 SETUP FAIL',13,10,'$'
transition_message db 'X87 PM CONTROL PASS',13,10,'$'
align 8
gdt:
    dq 0
code_descriptor: dw 0ffffh,0
    db 0,09ah,0,0
data_descriptor: dw 0ffffh,0
    db 0,092h,0,0
narrow_descriptor: dw 107,0
    db 0,092h,0,0
gdt_end:
gdtr dw gdt_end-gdt-1
    dd 0
idt:
    times 13 dq 0
    dw gp_handler,8
    db 0,086h
    dw 0
idt_end:
idtr dw idt_end-idt-1
    dd 0
align 4
arena times 128 db 0a5h
times 2048 db 0
stack_top:
