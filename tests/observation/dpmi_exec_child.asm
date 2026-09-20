; Disposable real-mode child for protected-mode EXEC acceptance.
bits 16
org 100h
%ifdef EXEC_LOAD_CALL
    cmp ax, 0BEEFh
    jne ordinary_exec
    mov ax, ds
    mov bx, cs
    cmp ax, bx
    jne call_failed
    cmp word [0], 20CDh
    jne call_failed
    cmp byte [80h], 3
    jne call_failed
    cmp word [81h], 4F20h
    jne call_failed
    cmp word [83h], 0D4Bh
    jne call_failed
    mov ax, 0C0DEh
    retf
call_failed:
    mov ax, 0BADh
    retf
ordinary_exec:
%endif
    cmp byte [80h], 3
    jne failed
    cmp word [81h], 4F20h
    jne failed
    cmp word [83h], 0D4Bh
    jne failed
%ifdef EXEC_INHERIT
    mov dx, child_control_c
    mov ax, 2523h
    int 21h
    mov ax, 3523h
    int 21h
    cmp bx, child_control_c
    jne failed
    mov ax, es
    mov bx, cs
    cmp ax, bx
    jne failed
    mov dx, child_critical_error
    mov ax, 2524h
    int 21h
    mov ax, 3524h
    int 21h
    cmp bx, child_critical_error
    jne failed
    mov ax, es
    mov bx, cs
    cmp ax, bx
    jne failed
    mov bx, 6
    mov ah, 3Eh
    int 21h
    jnc failed
    cmp ax, 6             ; Parent's no-inherit handle is absent here.
    jne failed
    mov bx, 5
    mov dx, inherited_payload
    mov cx, 4
    mov ah, 40h
    int 21h
    jc failed
    cmp ax, 4
    jne failed
    mov ah, 3Eh
    int 21h
    jc failed
%endif
    mov dx, success
    mov ah, 09h
    int 21h
    mov ax, 4C07h
    int 21h
failed:
    mov dx, failure
    mov ah, 09h
    int 21h
    mov ax, 4C63h
    int 21h
success db 'S38_EXEC_CHILD_TAIL_OK',13,10,'$'
%ifdef EXEC_INHERIT
inherited_payload db 'CHD!'
child_control_c:
    iret
child_critical_error:
    mov al, 3
    iret
%endif
failure db 'S38_FAIL_EXEC_CHILD_TAIL',13,10,'$'
