; Authored guest witness, not modified original guest media.
; Original emm_fncs.c AH56 -> spcemm.asm EmmRet/BOP68 -> INT67 return.
bits 16
org 100h
    push cs
    pop ds
    mov ah,41h
    int 67h
    or ah,ah
    jnz failed
    mov [frame],bx
    mov bx,2
    mov ah,43h
    int 67h
    or ah,ah
    jnz failed
    mov [handle],dx
    xor bx,bx
    call map
    mov es,[frame]
    mov word [es:0],0a55ah
    mov bx,1
    call map
    mov word [es:0],05aa5h
    xor bx,bx
    call map
    mov ax,cs
    mov [target_segment],ax
    mov [before_segment],ax
    mov [after_segment],ax
    mov [saved_sp],sp
    mov si,request
    mov dx,[handle]
    mov ax,5600h
    int 67h
    or ah,ah
    jnz failed
    cmp sp,[saved_sp]
    jne failed
    cmp byte [calls],1
    jne failed
    cmp byte [bad_map],0
    jne failed
    cmp word [es:0],0a55ah
    jne failed
    mov bx,1
    call map
    cmp word [es:0],06cc6h
    jne failed
    xor bx,bx
    call map
    ; Both refusals occur before any mapping or call-frame mutation.
    mov si,request
    mov dx,[handle]
    mov ax,5603h
    int 67h
    cmp ah,08fh                ; Original BAD_SUB_FUNC, not any error.
    jne failed
    cmp sp,[saved_sp]
    jne failed
    cmp word [es:0],0a55ah
    jne failed
    mov dx,0ffffh
    mov ax,5600h
    int 67h
    cmp ah,083h                ; Original BAD_HANDLE.
    jne failed
    cmp sp,[saved_sp]
    jne failed
    cmp word [es:0],0a55ah
    jne failed
    cmp byte [calls],1
    jne failed
    mov dx,[handle]
    mov ah,45h
    int 67h
    or ah,ah
    jnz failed
    mov dx,success
    mov ah,9
    int 21h
    mov ax,4c00h
    int 21h
map:
    mov dx,[handle]
    mov ax,4400h
    int 67h
    or ah,ah
    jnz failed
    ret
callee:
    inc byte [calls]
    cmp word [es:0],05aa5h
    je .mapped
    mov byte [bad_map],1
.mapped:
    mov word [es:0],06cc6h
    retf                       ; Actual driver traps this far return via BOP68.
failed:
    mov dx,failure
    mov ah,9
    int 21h
    mov ax,4c01h
    int 21h
frame dw 0
handle dw 0
saved_sp dw 0
calls db 0
bad_map db 0
request:
    dw callee
target_segment dw 0
    db 1
    dw before_map
before_segment dw 0
    db 1
    dw after_map
after_segment dw 0
before_map dw 1,0
after_map dw 0,0
success db 'T430_EMS_CALL_RETURN_STACK_MAP_OK',13,10,'$'
failure db 'T430_EMS_CALL_RETURN_FAIL',13,10,'$'
