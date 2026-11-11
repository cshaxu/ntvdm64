; Independently authored disposable real-mode witness. No original media patch.
bits 16
org 100h
    cli
    mov ax,cs
    mov ss,ax
    mov sp,stack_top
    sti
    mov ds,ax
    mov es,ax
    mov bx,(stack_top-$$+100h+15)/16
    mov ah,4ah
    int 21h
    jc failed
    mov ah,51h
    int 21h
    mov [record+4],bx
    mov ax,[16h]
    mov [record+6],ax
    call journal
%ifdef ROOT
    mov word [parameter+4],ds
    mov word [parameter+8],ds
    mov word [parameter+12],ds
%ifdef EXCLUSIONS
    mov dx,missing
    mov bx,parameter
    mov ax,4b00h
    int 21h
    jnc failed
    cmp ax,2
    jne failed
    mov byte [record+3],10
    mov [record+8],ax
    call journal
    mov dx,second
    mov bx,parameter
    mov ax,4b01h
    int 21h
    jc failed
    mov ax,cs
    mov ds,ax
    mov ax,[parameter+20] ; returned COM CS is its PSP
    mov [loaded_psp],ax
    mov es,ax
    mov ax,[es:2ch]
    mov [loaded_environment],ax
    mov bx,cs
    mov ah,50h           ; load-only changes current PSP; restore caller
    int 21h
    mov ax,[loaded_psp]
    mov es,ax
    mov ah,49h
    int 21h
    jc failed
    mov ax,[loaded_environment]
    cmp ax,[2ch]
    je .environment_done
    or ax,ax
    jz .environment_done
    mov es,ax
    mov ah,49h
    int 21h
    jc failed
.environment_done:
    mov ax,cs
    mov es,ax
    mov byte [record+3],11
    mov word [record+8],0
    call journal
    mov bx,128
    mov ah,48h
    int 21h
    jc failed
    mov [overlay],ax
    mov dx,second
    mov bx,overlay
    mov ax,4b03h
    int 21h
    jc failed
    mov ax,[overlay]
    mov es,ax
    cmp byte [es:0],0fah ; authored COM's first CLI byte was loaded
    jne failed
    mov ah,49h
    int 21h
    jc failed
    mov ax,cs
    mov es,ax
    mov byte [record+3],12
    mov word [record+8],0
    call journal
%endif
    mov dx,child
    call execute
    mov byte [record+3],2
    mov [record+8],ax
    call journal
%ifdef SECOND
    mov dx,second
    call execute
    mov byte [record+3],3
    mov [record+8],ax
    call journal
%endif
    mov dx,root_gate
    call gate
    mov ax,4c25h
    int 21h
%else
%ifndef FAST
    mov dx,child_gate
    call gate
%endif
%ifdef TSR
    mov dx,(stack_top-$$+100h+15)/16
    mov ax,3107h
%else
    mov ax,4c07h
%endif
    int 21h
%endif
execute:
    mov bx,parameter
    mov ax,4b00h
    int 21h
    jc failed
    mov ax,cs
    mov ds,ax
    mov es,ax
    mov ah,4dh
    int 21h
    cmp al,7
    jne failed
    ret
gate:
    push dx
.retry:
    pop dx
    push dx
    mov ax,3d00h
    int 21h
    jnc .opened
    int 28h
    jmp .retry
.opened:
    mov bx,ax
    mov ah,3eh
    int 21h
    pop dx
    ret
journal:
    mov dx,journal_path
    mov ax,3d42h
    int 21h
    jnc .opened
    xor cx,cx
    mov ah,3ch
    int 21h
    jc failed
.opened:
    mov bx,ax
    xor cx,cx
    xor dx,dx
    mov ax,4202h
    int 21h
    jc failed
    mov dx,record
    mov cx,10
    mov ah,40h
    int 21h
    jc failed
    cmp ax,10
    jne failed
    mov ah,3eh
    int 21h
    jc failed
    ret
failed:
    mov dx,failure
    mov ah,09h
    int 21h
    mov ax,4c63h
    int 21h
record dw 4344h
%ifdef ROOT
    db 'R',1
%elifdef FAST
    db 'F',1
%else
    db 'C',1
%endif
    dw 0,0,0
parameter dw 0,tail,0,5ch,0,6ch,0
    dw 0,0,0,0         ; EXEC load-only returned SS:SP/CS:IP storage
loaded_psp dw 0
loaded_environment dw 0
overlay dw 0,0
tail db 0,13
journal_path db 'Z:\tests\OBS\J.BIN',0
child db 'Z:\tests\OBS\C.COM',0
second db 'Z:\tests\OBS\F.COM',0
missing db 'Z:\tests\OBS\NOFILE.COM',0
child_gate db 'Z:\tests\OBS\GO0',0
root_gate db 'Z:\tests\OBS\GO1',0
failure db 'DOS-OBS-FAIL',13,10,'$'
times 256 db 0
stack_top:
