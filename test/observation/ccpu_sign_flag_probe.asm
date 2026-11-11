; Independently authored CPU40 guest witness; no original guest is modified.
bits 16
org 100h

%define ARITH_DF 0x0cd5

start:
    push cs
    pop ds
    push cs
    pop es
    cld
    pushfd
    pop esi
    and esi, ~ARITH_DF
    mov [flag_base], esi
    mov byte [round], 2
.sign_round:
    xor edi, edi
.sign_value:
    mov esi, edi
    and esi, 0x08d5
    or esi, [flag_base]

    mov byte [phase], 1
    mov ebx, edi
    test bx, 0x8000
    jz .cwde_positive
    or ebx, 0xffff0000
.cwde_positive:
    mov eax, [upper]
    or eax, edi
    push esi
    popfd
    cwde
    pushfd
    pop ecx
%ifdef CPU_PROBE_NEGATIVE_ORACLE
    xor ebx, 1 ; Deliberately wrong expectation, never a product mutation.
%endif
    cmp eax, ebx
    jne failed
    call check_sign_flags

    mov byte [phase], 2
    mov ebx, edi
    and ebx, 0xff
    test bl, 0x80
    jz .cbw_positive
    or ebx, 0xff00
.cbw_positive:
    or ebx, [upper]
    mov eax, [upper]
    or eax, edi
    push esi
    popfd
    cbw
    pushfd
    pop ecx
    cmp eax, ebx
    jne failed
    call check_sign_flags

    mov byte [phase], 3
    mov ebx, [upper]
    test di, 0x8000
    jz .cwd_positive
    or ebx, 0xffff
.cwd_positive:
    mov eax, [upper]
    or eax, edi
    mov ebp, eax
    mov edx, [upper]
    or edx, 0x39a6
    push esi
    popfd
    cwd
    pushfd
    pop ecx
    cmp edx, ebx
    jne failed
    cmp eax, ebp
    jne failed
    call check_sign_flags

    mov byte [phase], 4
    xor ebx, ebx
    test dword [upper], 0x80000000
    jz .cdq_positive
    dec ebx
.cdq_positive:
    mov eax, [upper]
    or eax, edi
    mov ebp, eax
    mov edx, 0x39a6c57e
    push esi
    popfd
    cdq
    pushfd
    pop ecx
    cmp edx, ebx
    jne failed
    cmp eax, ebp
    jne failed
    call check_sign_flags
    inc di
    jnz .sign_value
    xor dword [upper], 0xffff0000
    dec byte [round]
    jnz .sign_round
    mov dx, sign_pass
    call print
    xor edi, edi
.cdq_boundary:
    mov byte [phase], 4
    mov eax, [boundary_values+edi*4]
    mov ebp, eax
    xor ebx, ebx
    cmp eax, 0x7fffffff
    jbe .cdq_boundary_positive
    not ebx
.cdq_boundary_positive:
    mov esi, [flag_base]
    or esi, 0x8d5
    mov edx, 0x39a6c57e
    push esi
    popfd
    cdq
    pushfd
    pop ecx
    cmp edx, ebx
    jne failed
    cmp eax, ebp
    jne failed
    call check_sign_flags
    inc edi
    cmp edi, 8
    jb .cdq_boundary
    mov dx, boundary_pass
    call print

    xor edi, edi
.flag_value:
    mov eax, edi
    shr eax, 8
    mov esi, [flag_table + eax*4]
    or esi, [flag_base]
    test di, 1
    jz .df_clear
    or esi, 0x400
.df_clear:
    mov eax, edi
    and eax, 0xff
    shl eax, 8
    or eax, 0xa55a00e7
    mov ebp, eax

    mov byte [phase], 5
    mov ebx, esi
    and ebx, 0xd5
    or ebx, 2
    shl ebx, 8
    or ebx, 0xa55a00e7
    push esi
    popfd
    lahf
    pushfd
    pop ecx
    cmp eax, ebx
    jne failed
    call check_sign_flags

    mov byte [phase], 6
    mov eax, ebp
    mov ebx, esi
    and ebx, ~0xd5
    mov edx, edi
    and edx, 0xd5
    or ebx, edx
    push esi
    popfd
    sahf
    pushfd
    pop ecx
    cmp eax, ebp
    jne failed
    xor ecx, ebx
    and ecx, ARITH_DF
    jnz failed

%macro FLAG_CASE 4
    mov byte [phase], %1
    mov ebx, esi
    %2 ebx, %3
    push esi
    popfd
    %4
    pushfd
    pop ecx
    cmp eax, ebp
    jne failed
    xor ecx, ebx
    and ecx, ARITH_DF
    jnz failed
%endmacro
    FLAG_CASE 7, or, 1, stc
    FLAG_CASE 8, and, ~1, clc
    FLAG_CASE 9, xor, 1, cmc
    FLAG_CASE 10, or, 0x400, std
    FLAG_CASE 11, and, ~0x400, cld
    inc edi
    cmp edi, 16384
    jb .flag_value
    cld
    mov dx, flag_pass
    call print

    mov byte [phase], 12
    mov si, source_bytes+15
    mov di, destination+15
    mov cx, 16
    std
    rep movsb
    cld
    cmp si, source_bytes-1
    jne failed
    cmp di, destination-1
    jne failed
    mov si, source_bytes
    mov di, destination
    mov cx, 16
    repe cmpsb
    jne failed
    mov dx, pass
    call print
    mov ax, 0x4c00
    int 0x21

check_sign_flags:
    xor ecx, esi
    and ecx, ARITH_DF
    jnz failed
    ret
print:
    cld
    mov ah, 9
    int 0x21
    ret
failed:
    cld
    mov dx, failure
    call print
    mov dl, [phase]
    add dl, 'A'-1
    mov ah, 2
    int 0x21
    mov al, [phase]
    mov ah, 0x4c
    int 0x21

phase db 0
round db 0
upper dd 0xa55a0000
flag_base dd 0
boundary_values dd 0,1,0x7fffffff,0x80000000,0xffffffff,0xffff0000,0x7fff0000,0x8000ffff
flag_table:
%assign flag_index 0
%rep 64
    dd ((flag_index & 1) | ((flag_index & 2) << 1) | ((flag_index & 4) << 2) | ((flag_index & 8) << 3) | ((flag_index & 16) << 3) | ((flag_index & 32) << 6))
%assign flag_index flag_index+1
%endrep
source_bytes db 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15
destination times 16 db 0
sign_pass db 'CCPU SIGN PASS cases=524288',13,10,'$'
boundary_pass db 'CCPU CDQ BOUNDARIES PASS cases=8',13,10,'$'
flag_pass db 'CCPU FLAGS PASS cases=114688',13,10,'$'
pass db 'CCPU SIGN FLAGS REVERSE COPY PASS',13,10,'$'
failure db 'CCPU SIGN FLAGS FAIL phase=$'
