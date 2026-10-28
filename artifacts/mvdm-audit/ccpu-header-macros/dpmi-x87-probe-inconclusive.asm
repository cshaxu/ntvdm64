; Independent original-DOSX client. A 128-byte owned arena backs the narrow
; descriptor, so a guest limit violation never accesses unowned storage.
bits 16
org 100h
start:
    push cs
    pop ds
    mov sp,stack_top
    mov dx,start_message
    mov ah,9
    int 21h
    mov ax,ds
    movzx eax,ax
    shl eax,4
    add eax,arena
    mov [arena_base],eax
    mov bx,(image_end-$$+100h+15)/16
    mov ah,4ah
    int 21h
    jc setup_failed
    mov byte [setup_stage],'1'
    mov ax,1687h
    int 2fh
    test ax,ax
    jnz setup_failed
    mov [entry],di
    mov [entry+2],es
    mov bx,si
    mov byte [setup_stage],'2'
    test bx,bx
    jz .enter
    mov ah,48h
    int 21h
    jc setup_failed
    mov es,ax
.enter:
    mov byte [setup_stage],'3'
    xor ax,ax
    call far [entry]
    jc setup_failed
    mov dx,pm_message
    mov ah,9
    int 21h
    mov [client_ds],ds
    mov byte [setup_stage],'4'
    xor ax,ax
    mov cx,1
    int 31h
    jc setup_failed
    mov [selector],ax
    mov byte [setup_stage],'5'
    mov bx,ax
    mov ecx,[arena_base]
    mov edx,ecx
    shr ecx,16
    mov ax,0007h
    int 31h
    jc setup_failed
    mov byte [setup_stage],'6'
    mov cx,bx
    and cx,3
    shl cx,5
    or cx,0092h
    mov ax,0009h
    int 31h
    jc setup_failed
    mov byte [setup_stage],'7'
    mov ax,0202h
    mov bl,13
    int 31h
    jc setup_failed
    mov [old_handler],edx
    mov [old_handler+4],cx
    mov byte [setup_stage],'8'
    mov ax,0203h
    mov bl,13
    mov cx,cs
    mov edx,gp_handler
    int 31h
    jc setup_failed

    mov dx,107
    call set_limit
    call fill
    mov dx,ready_message
    mov ah,9
    int 21h
    fninit
    mov word [resume_ip],case1_done
    mov word [fault_ip],case1
case1:
    o32 fnsave [fs:0]
case1_done:
    cmp word [faults],0
    jne .bad
    mov si,arena+108
    mov cx,20
    call untouched
    jc .bad
    mov si,arena
    mov cx,108
    call written
    jc .bad
    mov dx,wide_pass
    jmp .wide_report
.bad:
    mov byte [mismatch],1
    mov dx,wide_fail
.wide_report:
    mov ah,9
    int 21h

    mov dx,93
    call set_limit
    call fill
    fninit
    mov word [resume_ip],case2_done
    mov word [fault_ip],case2
case2:
    o32 fnsave [fs:0]
case2_done:
    cmp word [faults],1
    jne .bad
    mov si,arena
    mov cx,128
    call untouched
    jc .bad
    mov dx,narrow32_pass
    jmp .report
.bad:
    mov byte [mismatch],1
    mov dx,narrow32_fail
.report:
    mov ah,9
    int 21h

    call fill
    fninit
    mov word [resume_ip],case3_done
    mov word [fault_ip],case3
case3:
    o16 fnsave [fs:0]
case3_done:
    cmp word [faults],0
    jne .bad
    mov si,arena+94
    mov cx,34
    call untouched
    jc .bad
    mov si,arena
    mov cx,94
    call written
    jc .bad
    mov dx,narrow16_pass
    jmp .report
.bad:
    mov byte [mismatch],1
    mov dx,narrow16_fail
.report:
    mov ah,9
    int 21h
    cmp byte [bad_frame],0
    je .cleanup
    mov byte [mismatch],1
.cleanup:
    mov ax,0203h
    mov bl,13
    mov edx,[old_handler]
    mov cx,[old_handler+4]
    int 31h
    jc setup_failed
    xor ax,ax
    mov fs,ax
    mov bx,[selector]
    mov ax,0001h
    int 31h
    jc setup_failed
    xor ax,ax
    mov al,[mismatch]
    mov ah,4ch
    int 21h

set_limit:
    mov byte [setup_stage],'9'
    mov bx,[selector]
    xor cx,cx
    mov ax,0008h
    int 31h
    jc setup_failed
    mov ax,[selector]
    mov fs,ax                 ; Reload the actual segment cache after mutation.
    ret
fill:
    push es
    push ds
    pop es
    cld
    mov di,arena
    mov cx,128
    mov al,0a5h
    rep stosb
    pop es
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
    push ds
    mov ax,[cs:client_ds]
    mov ds,ax
    inc word [faults]
    mov ax,[ss:bp+8]
    cmp ax,[fault_ip]
    jne .bad
    mov ax,cs
    cmp ax,[ss:bp+10]
    jne .bad
    cmp word [ss:bp+6],0
    je .resume
.bad:
    mov byte [bad_frame],1
.resume:
    mov ax,[resume_ip]
    mov [ss:bp+8],ax
    pop ds
    pop ax
    pop bp
    retf                     ; Original 16-bit DPMI exception return frame.
setup_failed:
    mov dx,setup_message
    mov ah,9
    int 21h
    mov dl,[setup_stage]
    mov ah,2
    int 21h
    mov ax,4c02h
    int 21h

entry dd 0
arena_base dd 0
old_handler dd 0
dw 0
selector dw 0
client_ds dw 0
faults dw 0
fault_ip dw 0
resume_ip dw 0
setup_stage db '0'
bad_frame db 0
mismatch db 0
wide_pass db 'X87 WIDE32 PASS',13,10,'$'
wide_fail db 'X87 WIDE32 FAIL',13,10,'$'
narrow32_pass db 'X87 NARROW32 PASS',13,10,'$'
narrow32_fail db 'X87 NARROW32 FAIL',13,10,'$'
narrow16_pass db 'X87 NARROW16 PASS',13,10,'$'
narrow16_fail db 'X87 NARROW16 FAIL',13,10,'$'
setup_message db 'X87 SETUP FAIL stage=','$'
start_message db 'X87 START',13,10,'$'
pm_message db 'X87 PM ENTERED',13,10,'$'
ready_message db 'X87 SAVE READY',13,10,'$'
align 4
arena times 128 db 0a5h
times 2048 db 0
stack_top:
image_end:
