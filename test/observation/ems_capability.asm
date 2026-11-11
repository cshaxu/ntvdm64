; S30 test-only DOS guest.  It executes the original EMM interface through
; INT 67h: status, page-frame lookup, allocate, map, remap, unmap and free.
bits 16
org 100h

start:
    ; This PIF loads HIMEM.SYS but its task AUTOEXEC.NT does not load DOSX.
    ; Prove the original PIF profile override before entering the EMM path.
    mov ax, 4300h
    int 2Fh
    cmp al, 80h
    jne fail
    mov ax, 1687h
    int 2Fh
    or ax, ax
    jz fail

    mov ah, 40h                    ; Get EMM status
    int 67h
    or ah, ah
    jnz fail

    mov ah, 41h                    ; Get page-frame segment
    int 67h
    or ah, ah
    jnz fail
    mov [frame_segment], bx

    mov bx, 2                      ; Allocate two 16 KiB logical pages
    mov ah, 43h
    int 67h
    or ah, ah
    jnz fail
    mov [handle], dx

    mov al, 0                      ; Physical frame 0 <- logical page 0
    xor bx, bx
    mov dx, [handle]
    mov ah, 44h
    int 67h
    or ah, ah
    jnz fail_free
    mov es, [frame_segment]
    mov byte [es:0], 5ah

    mov al, 0                      ; Same frame <- distinct logical page 1
    mov bx, 1
    mov dx, [handle]
    mov ah, 44h
    int 67h
    or ah, ah
    jnz fail_free
    cmp byte [es:0], 0
    jne fail_free

    mov al, 0                      ; Restore page 0 and prove alias content
    xor bx, bx
    mov dx, [handle]
    mov ah, 44h
    int 67h
    or ah, ah
    jnz fail_free
    cmp byte [es:0], 5ah
    jne fail_free

    mov al, 0                      ; Unmap physical frame 0
    mov bx, 0ffffh
    mov dx, [handle]
    mov ah, 44h
    int 67h
    or ah, ah
    jnz fail_free
    mov dx, [handle]
    mov ah, 45h                    ; Deallocate the handle
    int 67h
    or ah, ah
    jnz fail
    mov dx, profile_text
    mov ah, 09h
    int 21h
    mov dx, ok_text
    jmp short print_exit

fail_free:
    mov dx, [handle]
    mov ah, 45h
    int 67h
fail:
    mov dx, fail_text
print_exit:
    mov ah, 09h
    int 21h
    mov ax, 4c00h
    int 21h

frame_segment dw 0
handle        dw 0
profile_text  db 'S30_HIMEM_ONLY_OK ', '$'
ok_text       db 'S30_EMS_STATUS_OK S30_EMS_MAP_OK S30_EMS_UNMAP_OK S30_EMS_FREE_OK',13,10,'$'
fail_text     db 'S30_EMS_FAIL',13,10,'$'
