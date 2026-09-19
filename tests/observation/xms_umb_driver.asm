; Test-only character driver: runs after HIMEM but before DOS ConfigDone
; claims the remaining UMBs. It allocates, releases and rejects double free
; through HIMEM's public entry; it neither supplies nor patches XMS behavior.
bits 16
org 0
    dd 0FFFFFFFFh
    dw 8000h
    dw strategy, interrupt
    db 'X35UMB  '
request dd 0
entry dd 0
handle dw 0
strategy:
    mov [cs:request], bx
    mov [cs:request+2], es
    retf
interrupt:
    pushf
    pusha
    push ds
    push es
    push cs
    pop ds
    les bx, [request]
    mov word [es:bx+3], 0100h
    cmp byte [es:bx+2], 0
    jne done
%ifdef STARTUP_FAULT
    mov dx, fault_path
    xor cx, cx
    mov ah, 3Ch
    int 21h
    jc fault_witness_failed
    mov bx, ax
    mov dx, fault_marker
    mov cx, fault_marker_end-fault_marker-1
    mov ah, 40h
    int 21h
    pushf
    mov ah, 3Eh
    int 21h
    popf
    jc fault_witness_failed
    mov dx, fault_marker
    mov ah, 09h
    int 21h
    db 0Fh, 0Bh                  ; deliberate #UD, never original guest media
fault_witness_failed:
    jmp failure
%endif
    mov ax, 4300h
    int 2Fh
    cmp al, 80h
    jne failure
    mov ax, 4310h
    int 2Fh
    mov [entry], bx
    mov [entry+2], es
    mov dx, 0FFFFh
    mov ah, 10h
    call far [entry]
    test ax, ax
    jnz failure
    cmp bl, 0B0h
    jne failure
    mov dx, 16
    mov ah, 10h
    call far [entry]
    cmp ax, 1
    jne failure
    mov [handle], bx
    mov dx, bx
    mov ah, 11h
    call far [entry]
    cmp ax, 1
    jne failure
    mov dx, [handle]
    mov ah, 11h
    call far [entry]
    test ax, ax
    jnz failure
    cmp bl, 0B2h
    jne failure
    ; Read-only A20 witness before DOS claims HMA. Never write IVT or HMA.
    pushf
    cli
    mov ah, 07h
    call far [entry]
    mov [initial_a20], ax
    mov ah, 05h
    call far [entry]
    cmp ax, 1
    jne a20_failed
    call compare_a20_alias
    setz byte [a20_bad]           ; enabled: low IVT and HMA must differ
    mov ah, 06h
    call far [entry]
    cmp ax, 1
    jne a20_failed
    mov ah, 07h
    call far [entry]
    cmp ax, [initial_a20]
    jne a20_failed
    or ax, ax
    jnz a20_checked
    call compare_a20_alias
    jne a20_failed               ; disabled: exact 1MB alias must match
a20_checked:
    popf
    cmp byte [a20_bad], 0
    jne failure
    mov dx, a20_success
    cmp word [initial_a20], 0
    jne a20_print
    mov dx, a20_wrap_success
a20_print:
    mov ah, 09h
    int 21h
    mov dx, success
    jmp show
a20_failed:
    popf
    jmp failure
failure:
    mov dx, failed
show:
    mov ah, 09h
    int 21h
    les bx, [request]
    ; Discard the test driver after initialization; no resident resources.
    mov word [es:bx+14], 0
    mov [es:bx+16], cs
done:
    pop es
    pop ds
    popa
    popf
    retf
success db 'S35_XMS_BOOT_UMB_ALLOC_RELEASE_DOUBLE_FREE_OK',13,10,'$'
a20_success db 'S35_XMS_BOOT_A20_ALIAS_STATE_RESTORED_OK',13,10,'$'
a20_wrap_success db 'S35_XMS_BOOT_A20_OFF_ON_OFF_ALIAS_OK',13,10,'$'
initial_a20 dw 0
a20_bad db 0
compare_a20_alias:
    push ds
    push es
    push si
    push di
    push cx
    xor ax, ax
    mov ds, ax
    dec ax
    mov es, ax
    xor si, si
    mov di, 10h
    mov cx, 128
    cld
    repe cmpsw
    pop cx
    pop di
    pop si
    pop es
    pop ds
    ret
failed db 'S35_XMS_BOOT_UMB_FAIL',13,10,'$'
%ifdef STARTUP_FAULT
fault_marker db 'S35_TEST_DRIVER_STARTUP_FAULT',13,10,'$'
fault_marker_end:
fault_path db 'O:\WINNT\TESTS\S35F.HIT',0
%endif
