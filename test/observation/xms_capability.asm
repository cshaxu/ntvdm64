; T420 S35 test-only DOS guest.  It reaches the public HIMEM/XMS entry
; through INT 2Fh, then proves allocation, real-mode/XMS moves, reallocation,
; free and local A20 state through the selected original XMS package.

bits 16
org 100h

start:
    push cs
    pop ds

    mov ax, 4300h
    int 2Fh
    cmp al, 80h
    jne fail_entry
    mov ax, 4310h
    int 2Fh
    mov [xms_entry], bx
    mov [xms_entry + 2], es

    mov ah, 00h                    ; XMS version
    call far [xms_entry]
    or ax, ax
    jz fail_version

    mov ah, 08h                    ; Query largest/total free extended RAM
    call far [xms_entry]
    or ax, ax
    jz fail_query
    or dx, dx
    jz fail_query
    mov [initial_free_kb], dx

    mov dx, 64                     ; Allocate 64 KiB extended block
    mov ah, 09h
    call far [xms_entry]
    cmp ax, 1
    jne fail_alloc
    mov [xms_handle], dx

%ifdef FORCED_RELOCATION
    ; S36: occupy the following extent and prove it is actually adjacent.
    mov ah, 0Ch
    call far [xms_entry]
    cmp ax, 1
    jne fail_free
    mov [original_base], bx
    mov [original_base + 2], dx
    mov dx, [xms_handle]
    mov ah, 0Dh
    call far [xms_entry]
    cmp ax, 1
    jne fail_free
    mov dx, 64
    mov ah, 09h
    call far [xms_entry]
    cmp ax, 1
    jne fail_free
    mov [guard_handle], dx
    mov ah, 0Ch
    call far [xms_entry]
    cmp ax, 1
    jne fail_free
    movzx eax, dx
    shl eax, 16
    mov ax, bx
    sub eax, [original_base]
    cmp eax, 65536
    jne fail_free
    mov dx, [guard_handle]
    mov ah, 0Dh
    call far [xms_entry]
    cmp ax, 1
    jne fail_free
    mov dx, [xms_handle]
%endif

    ; Move eight bytes from this conventional COM image into the XMS block.
    mov word [move_src_handle], 0
    mov word [move_dst_handle], dx
    mov word [move_src_offset], source_bytes
    mov [move_src_offset + 2], ds
    mov dword [move_dst_offset], 0
    mov si, move_length
    mov ah, 0Bh
    call far [xms_entry]
    cmp ax, 1
    jne fail_free

    ; Reallocate to exercise the original allocation/copy/lease transition.
    mov dx, [xms_handle]
    mov bx, 128
    mov ah, 0Fh
    call far [xms_entry]
    cmp ax, 1
    jne fail_free

    ; Move the original bytes back to conventional memory and compare.
%ifdef FORCED_RELOCATION
    mov dx, [xms_handle]
    mov ah, 0Ch
    call far [xms_entry]
    cmp ax, 1
    jne fail_free
    movzx eax, dx
    shl eax, 16
    mov ax, bx
    cmp eax, [original_base]
    je fail_free                    ; successful resize alone is insufficient
    mov dx, [xms_handle]
    mov ah, 0Dh
    call far [xms_entry]
    cmp ax, 1
    jne fail_free
    mov dx, [guard_handle]
    mov ah, 0Ah
    call far [xms_entry]
    cmp ax, 1
    jne fail_free
    mov word [guard_handle], 0
%endif
    mov dx, [xms_handle]
    mov [move_src_handle], dx
    mov word [move_dst_handle], 0
    mov dword [move_src_offset], 0
    mov word [move_dst_offset], source_bytes
    mov [move_dst_offset + 2], ds
    mov dword [source_bytes], 0
    mov dword [source_bytes + 4], 0
    mov si, move_length
    mov ah, 0Bh
    call far [xms_entry]
    cmp ax, 1
    jne fail_free
    cmp dword [source_bytes], 'XMSM'
    jne fail_free
    cmp dword [source_bytes + 4], 'OVES'
    jne fail_free

    ; Same-size requests are an original HIMEM no-op, before the host BOP.
    mov dx, [xms_handle]
    mov bx, 128
    mov ah, 0Fh
    call far [xms_entry]
    cmp ax, 1
    jne fail_free

    ; Failed growth must preserve the old allocation and its data.
    mov dx, [xms_handle]
    mov bx, 0FFFFh
    mov ah, 0Fh
    call far [xms_entry]
    or ax, ax
    jnz fail_free
    cmp bl, 0A0h
    jne fail_free
    mov dx, [xms_handle]
    mov ah, 0Eh
    call far [xms_entry]
    cmp ax, 1
    jne fail_free
    cmp dx, 128
    jne fail_free

    ; Shrink through the same public owner; retain the leading data.
    mov dx, [xms_handle]
    mov bx, 32
    mov ah, 0Fh
    call far [xms_entry]
    cmp ax, 1
    jne fail_free
    mov dword [source_bytes], 0
    mov dword [source_bytes + 4], 0
    mov si, move_length
    mov ah, 0Bh
    call far [xms_entry]
    cmp ax, 1
    jne fail_free
    cmp dword [source_bytes], 'XMSM'
    jne fail_free
    cmp dword [source_bytes + 4], 'OVES'
    jne fail_free

    ; Original contract guarantees forward overlap (destination < source).
    mov word [move_src_handle], 0
    mov word [move_dst_handle], 0
    mov word [move_src_offset], source_bytes + 2
    mov [move_src_offset + 2], ds
    mov word [move_dst_offset], source_bytes
    mov [move_dst_offset + 2], ds
    mov dword [move_length], 6
    mov si, move_length
    mov ah, 0Bh
    call far [xms_entry]
    cmp ax, 1
    jne fail_free
    cmp dword [source_bytes], 'SMOV'
    jne fail_free
    cmp word [source_bytes + 4], 'ES'
    jne fail_free

    ; An odd transfer length must fail without changing the buffer.
    mov dword [move_length], 3
    mov si, move_length
    mov ah, 0Bh
    call far [xms_entry]
    or ax, ax
    jnz fail_free
    cmp bl, 0A7h
    jne fail_free

    mov dx, [xms_handle]
    mov ah, 0Ch                    ; Lock block, receive linear address
    call far [xms_entry]
    cmp ax, 1
    jne fail_free
    mov dx, [xms_handle]
    mov ah, 0Eh                    ; Query lock count and new block size
    call far [xms_entry]
    cmp ax, 1
    jne fail_free
    cmp bh, 1
    jne fail_free
    cmp dx, 32
    jne fail_free
    mov dx, [xms_handle]
    mov bx, 64
    mov ah, 0Fh                    ; Reallocation of a locked block is refused
    call far [xms_entry]
    or ax, ax
    jnz fail_free
    cmp bl, 0ABh
    jne fail_free
    mov dx, [xms_handle]
    mov ah, 0Dh
    call far [xms_entry]
    cmp ax, 1
    jne fail_free
    mov dx, [xms_handle]
    mov ah, 0Dh                    ; Second unlock reports already unlocked
    call far [xms_entry]
    or ax, ax
    jnz fail_free
    cmp bl, 0AAh
    jne fail_free

    ; Local A20 nesting must be handled by the original XMS/SAS route.
    mov ah, 05h
    call far [xms_entry]
    cmp ax, 1
    jne fail_free
    mov ah, 07h
    call far [xms_entry]
    cmp ax, 1
    jne fail_free
    mov ah, 06h
    call far [xms_entry]
    cmp ax, 1
    jne fail_free

    mov dx, [xms_handle]
    mov ah, 0Ah
    call far [xms_entry]
    cmp ax, 1
    jne fail_free

    mov dx, [xms_handle]
    mov ah, 0Ah                    ; Double free rejects the retired handle
    call far [xms_entry]
    or ax, ax
    jnz fail_move
    cmp bl, 0A2h
    jne fail_move
    mov word [xms_handle], 0

    mov dx, 0FFFFh                 ; Beyond this worker's memory budget
    mov ah, 09h
    call far [xms_entry]
    or ax, ax
    jnz fail_free
    cmp bl, 0A0h
    jne fail_move

    mov dx, 16                     ; Reuse after release
    mov ah, 09h
    call far [xms_entry]
    cmp ax, 1
    jne fail_alloc
    mov [xms_handle], dx
    mov ah, 0Ah
    call far [xms_entry]
    cmp ax, 1
    jne fail_free
    mov word [xms_handle], 0

    ; Query UMB availability via an intentionally oversized allocation.
    mov ah, 08h
    call far [xms_entry]
    cmp dx, [initial_free_kb]
    jne fail_query                 ; all test-owned XMS capacity restored
    mov dx, 0FFFFh
    mov ah, 10h
    call far [xms_entry]
    or ax, ax
    jnz fail_move
    cmp bl, 0B1h
    je umb_unavailable
    cmp bl, 0B0h
    jne fail_move
    mov dx, 16
    mov ah, 10h
    call far [xms_entry]
    cmp ax, 1
    jne fail_move
    mov dx, bx
    mov [umb_handle], bx
    mov ah, 11h
    call far [xms_entry]
    cmp ax, 1
    jne fail_move
    mov dx, [umb_handle]
    mov ah, 11h
    call far [xms_entry]
    or ax, ax
    jnz fail_move
    cmp bl, 0B2h
    jne fail_move
    mov dx, umb_ok_text
    jmp short print_umb
umb_unavailable:
    mov dx, umb_absent_text
print_umb:
    mov ah, 09h
    int 21h
%ifdef UMB_PROFILE
    mov ah, 88h                    ; HIMEM /INT15=128 reserved memory
    clc                            ; HIMEM's original IRET preserves flags
    int 15h
    push ax
    pushf
    call print_hex
    popf
    pop ax
    jc fail_move
%ifdef DEFAULT_INT15
    cmp ax, 0
%else
    cmp ax, 128
%endif
    jne fail_move
    mov dx, int15_text
    mov ah, 09h
    int 21h
%endif
    mov dx, success_text
    jmp short print_success

fail_free:
    mov dx, [xms_handle]
    or dx, dx
    jz fail_move
    mov ah, 0Ah
    call far [xms_entry]
fail_move:
    mov dx, fail_move_text
    jmp short print_failure
fail_entry:
    mov dx, fail_entry_text
    jmp short print_failure
fail_version:
    mov dx, fail_version_text
    jmp short print_failure
fail_query:
    mov dx, fail_query_text
    jmp short print_failure
fail_alloc:
    mov dx, fail_alloc_text
print_failure:
    mov ah, 09h
    int 21h
    mov ax, 4C01h
    int 21h
print_success:
    mov ah, 09h
    int 21h
    mov ax, 4C00h
    int 21h

; Print the observed register without altering the tested return state.
print_hex:
    pusha
    mov bx, ax
    mov cx, 4
.digit:
    rol bx, 4
    mov dl, bl
    and dl, 0Fh
    add dl, '0'
    cmp dl, '9'
    jbe .emit
    add dl, 7
.emit:
    mov ah, 02h
    int 21h
    loop .digit
    mov dl, 13
    int 21h
    mov dl, 10
    int 21h
    popa
    ret

xms_entry       dw 0, 0
%ifdef FORCED_RELOCATION
original_base   dd 0
guard_handle    dw 0
%endif
xms_handle      dw 0
initial_free_kb dw 0
umb_handle      dw 0
move_length     dd 8
move_src_handle dw 0
move_src_offset dd 0
move_dst_handle dw 0
move_dst_offset dd 0
source_bytes    db 'XMSMOVES'

%ifdef FORCED_RELOCATION
success_text      db 'S36_XMS_FORCED_RELOCATION_DATA_RELEASE_OK',13,10,'$'
%else
success_text      db 'S35_XMS_ALLOC_MOVE_REALLOC_FREE_A20_OK',13,10,'$'
%endif
umb_ok_text       db 'S35_XMS_UMB_ALLOC_FREE_OK',13,10,'$'
umb_absent_text   db 'S35_XMS_UMB_NO_FREE_BLOCKS',13,10,'$'
%ifdef DEFAULT_INT15
int15_text        db 'S35_XMS_INT15_DEFAULT_ZERO_OK',13,10,'$'
%else
int15_text        db 'S35_XMS_INT15_RESERVED_OK',13,10,'$'
%endif
fail_entry_text   db 'S35_XMS_FAIL_ENTRY',13,10,'$'
fail_version_text db 'S35_XMS_FAIL_VERSION',13,10,'$'
fail_query_text   db 'S35_XMS_FAIL_QUERY',13,10,'$'
fail_alloc_text   db 'S35_XMS_FAIL_ALLOC',13,10,'$'
fail_move_text    db 'S35_XMS_FAIL_MOVE_OR_A20',13,10,'$'
