; Independent DOS parent: verify original DPMI child-exit cleanup in one worker.
bits 16
org 100h
start:
    mov sp, stack_top
    mov bx, (image_end - $$ + 100h + 15) / 16
    mov ah, 4Ah
    int 21h
    jc failed
    mov ax, 4310h
    int 2Fh
    mov [xms], bx
    mov [xms+2], es
    mov ax, ds
    mov [exec_tail_seg], ax
    mov [exec_fcb1_seg], ax
    mov [exec_fcb2_seg], ax
    ; Warm original DOSX-owned persistent tables before measuring child leaks.
    mov dx, warm_image
    call execute
    jc failed
    call query
    test ax, ax
    jz failed
    test dx, dx
    jz failed
    mov [largest], ax
    mov [available], dx
    mov byte [remaining], 4
.loop:
    mov dx, live_image
    call execute
    jc failed
    call query
    cmp ax, [largest]
    jne failed
    cmp dx, [available]
    jne failed
    dec byte [remaining]
    jnz .loop
    mov dx, success
    mov ah, 09h
    int 21h
    mov ax, 4C00h
    int 21h
query:
    mov ah, 08h
    call far [xms]
    ret
execute:
    push ds
    pop es
    mov bx, exec_block
    mov ax, 4B00h
    push ds
    int 21h
    pop ds
    jc .done
    mov ah, 4Dh
    int 21h
    test ax, ax
    jnz .error
    clc
.done:
    ret
.error:
    stc
    ret
failed:
    mov dx, failure
    mov ah, 09h
    int 21h
    mov ax, 4C01h
    int 21h
xms dd 0
largest dw 0
available dw 0
remaining db 0
exec_block dw 0
    dw tail
exec_tail_seg dw 0
    dw 5Ch
exec_fcb1_seg dw 0
    dw 6Ch
exec_fcb2_seg dw 0
tail db 0,13
warm_image db 'tests\D36N.COM',0
live_image db 'tests\D36L.COM',0
success db 'S36_DPMI_TASK_EXIT_CAPACITY_RESTORED_OK',13,10,'$'
failure db 'S36_DPMI_TASK_EXIT_FAIL',13,10,'$'
times 2048 db 0
stack_top:
image_end:
