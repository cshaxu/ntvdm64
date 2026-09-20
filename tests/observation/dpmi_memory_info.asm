; Independent original INT31/0500 -> BOP 53:0B acceptance.
bits 16
org 100h
%ifdef LIMIT_PROBE
%ifndef HIGH_OFFSET
%error LIMIT_PROBE requires HIGH_OFFSET
%endif
%endif
%ifdef HIGH_OFFSET
%ifndef CLIENT32
%error HIGH_OFFSET requires CLIENT32
%endif
%endif
    mov sp, stack_top
    mov bx, (image_end - $$ + 100h + 15) / 16
    mov ah, 4Ah
    int 21h
    jc failed
    mov ax, 1687h
    int 2Fh
    test ax, ax
    jnz failed
    mov [entry], di
    mov [entry+2], es
    mov bx, si
    test bx, bx
    jz .enter
    mov ah, 48h
    int 21h
    jc failed
    mov es, ax
.enter:
%ifdef CLIENT32
    mov ax, 1
%else
    xor ax, ax
%endif
    call far [entry]
    jc failed
    push ds
    pop es
%ifdef HIGH_OFFSET
    mov bx, 2
    xor cx, cx
    mov ax, 0501h
    int 31h
    jc failed
    mov [buffer_handle], di
    mov [buffer_handle+2], si
    mov [buffer_base], cx
    mov [buffer_base+2], bx
    xor ax, ax
    mov cx, 1
    int 31h
    jc failed
    mov [buffer_selector], ax
    mov bx, ax
    mov cx, [buffer_base+2]
    mov dx, [buffer_base]
    mov ax, 0007h
    int 31h
    jc failed
    mov bx, [buffer_selector]
    mov cx, 1
    mov dx, 0FFFFh
    mov ax, 0008h
    int 31h
    jc failed
    mov es, [buffer_selector]
%endif
    call query
    mov eax, [info]
    mov [largest], eax
    mov eax, [info+20]
    mov [pages], eax
    mov bx, 1
    xor cx, cx
    mov ax, 0501h
    int 31h
    jc failed
    mov [handle], di
    mov [handle+2], si
    call query
    mov eax, [pages]
    sub eax, 16
    cmp [info+20], eax        ; Exactly 64K leaves the shared pool.
    jne failed
    mov eax, [info]
    cmp eax, [largest]
    ja failed
    mov si, [handle+2]
    mov di, [handle]
    mov ax, 0502h
    int 31h
    jc failed
    call query
    mov eax, [info]
    cmp eax, [largest]
    jne failed
    mov eax, [info+20]
    cmp eax, [pages]
    jne failed
%ifdef HIGH_OFFSET
    push ds
    pop es
    mov bx, [buffer_selector]
    mov ax, 0001h
    int 31h
    jc failed
    mov si, [buffer_handle+2]
    mov di, [buffer_handle]
    mov ax, 0502h
    int 31h
    jc failed
%endif
    mov edx, success
    mov ah, 09h
    int 21h
    mov ax, 4C00h
    int 21h
query:
%ifdef HIGH_OFFSET
    mov edi, 0FFFCh
    mov eax, 12345678h
    a32 stosd
    mov eax, 0A5A5A5A5h
    mov ecx, 12
    a32 rep stosd
    mov eax, 87654321h
    a32 stosd
%ifdef LIMIT_PROBE
    ; Keep backing memory allocated, but expose only 16 bytes of the output
    ; through ES. This diagnostic never points outside test-owned memory.
    mov bx, [buffer_selector]
    mov cx, 1
    mov dx, 000Fh
    mov ax, 0008h
    int 31h
    jc failed
    mov bx, [buffer_selector]
    lsl eax, bx
    jnz failed
    cmp eax, 1000Fh
    jne failed
%endif
    mov edi, 10000h
%else
    mov edi, info
%endif
    mov ax, 0500h
    int 31h
    jc failed
%ifdef LIMIT_PROBE
    ; Restore the test descriptor before inspecting the complete owned block.
    mov bx, [buffer_selector]
    mov cx, 1
    mov dx, 0FFFFh
    mov ax, 0008h
    int 31h
    jc failed
%endif
%ifdef HIGH_OFFSET
    mov esi, 0FFFCh
    mov edi, guard_before
    mov ecx, 14
.copy_info:
    mov eax, [es:esi]
    mov [edi], eax
    add esi, 4
    add edi, 4
    dec ecx
    jnz .copy_info
%endif
    cmp dword [guard_before], 12345678h
    jne failed
    cmp dword [guard_after], 87654321h
    jne failed
    mov eax, [info]
    test eax, eax
    jz failed
    shr eax, 12
    cmp [info+4], eax
    jne failed
    cmp dword [info+8], 0B61h ; Original compatibility constants, not real locks.
    jne failed
    cmp dword [info+12], 4096
    jne failed
    cmp dword [info+16], 0B68h
    jne failed
    cmp dword [info+24], 4096
    jne failed
    mov eax, [info+20]
    cmp [info+28], eax
    jne failed
    cmp eax, [info+4]
    jb failed
    cmp dword [info+32], 000FFFFFh ; DWORD GlobalMemoryStatus bytes / 4096.
    ja failed
    ; Original host structure has nine DWORDs, not twelve. Observe its exact
    ; write extent; reserved tail is not reinterpreted as implemented fields.
    cmp dword [info+36], 0A5A5A5A5h
    jne failed
    cmp dword [info+40], 0A5A5A5A5h
    jne failed
    cmp dword [info+44], 0A5A5A5A5h
    jne failed
%ifdef LIMIT_PROBE
    ; All 36 returned bytes passed their value checks despite the 16-byte
    ; descriptor window. Report a defect, never an acceptance success.
    mov edx, limit_failure
    mov ah, 09h
    int 21h
    mov ax, 4C01h
    int 21h
%endif
    ret
failed:
    mov edx, failure
    mov ah, 09h
    int 21h
    mov ax, 4C01h
    int 21h
entry dd 0
handle dd 0
largest dd 0
pages dd 0
%ifdef HIGH_OFFSET
buffer_handle dd 0
buffer_base dd 0
buffer_selector dw 0
%endif
guard_before dd 12345678h
info times 48 db 0A5h
guard_after dd 87654321h
success:
%ifdef HIGH_OFFSET
    db 'S38_MEMORY_INFO_HIGH_EDI_OK',13,10
%endif
%ifdef CLIENT32
    db 'S38_MEMORY_INFO_CLIENT32_OK',13,10
%endif
    db 'S38_MEMORY_INFO_ALLOC_FREE_FIELDS_OK',13,10,'$'
failure db 'S38_FAIL_MEMORY_INFO',13,10,'$'
%ifdef LIMIT_PROBE
limit_failure db 'S38_FAIL_MEMORY_INFO_WROTE_PAST_ES_LIMIT',13,10,'$'
%endif
times 4096 db 0
stack_top:
image_end:
