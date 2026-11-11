; Independent original CPU40 NOT/XCHG/LEA witness, using validated integer
; instructions to compute expectations before restoring the tested flags.
bits 16
org 100h
jmp start

%macro FLAGS_START 0
    push dword [flags]
    popfd
%endmacro
%macro FLAGS_CHECK 0
    pushfd
    pop edx
    xor edx,[flags]
    and edx,0x0cd5
    jnz failed
%endmacro

%macro NOT_WIDTH 3
    mov byte [phase],1
    mov eax,[left]
    mov ebx,eax
    xor ebx,%3
%ifdef CPU_TRANSFER_NEGATIVE
    xor ebx,1
%endif
    FLAGS_START
    not %1
    FLAGS_CHECK
    cmp eax,ebx
    jne failed
    inc dword [checks]
%assign offset 0
%rep 2
    mov eax,[left]
    mov [scratch+offset],eax
    mov byte [scratch+offset-1],0x69
    mov byte [scratch+offset+4],0x96
    FLAGS_START
    not %2 [scratch+offset]
    FLAGS_CHECK
    cmp eax,[left]
    jne failed
    cmp dword [scratch+offset],ebx
    jne failed
    cmp byte [scratch+offset-1],0x69
    jne failed
    cmp byte [scratch+offset+4],0x96
    jne failed
    inc dword [checks]
%assign offset offset+1
%endrep
%endmacro

%macro EXCHANGE_WIDTH 4
    mov byte [phase],2
    mov eax,[left]
    mov ebx,[right]
    mov ecx,eax
    xor ecx,ebx
    and ecx,%4
    xor eax,ecx
    xor ebx,ecx
    mov [expected_left],eax
    mov [expected_right],ebx
    mov eax,[left]
    mov ebx,[right]
    FLAGS_START
    xchg %1,%2
    FLAGS_CHECK
    cmp eax,[expected_left]
    jne failed
    cmp ebx,[expected_right]
    jne failed
    inc dword [checks]

%assign offset 0
%rep 2
    mov eax,[left]
    mov ebx,[right]
    mov [scratch+offset],ebx
    mov byte [scratch+offset-1],0x69
    mov byte [scratch+offset+4],0x96
    FLAGS_START
    xchg %1,%3 [scratch+offset]
    FLAGS_CHECK
    cmp eax,[expected_left]
    jne failed
    cmp ebx,[right]
    jne failed
    mov ecx,[scratch+offset]
    cmp ecx,[expected_right]
    jne failed
    cmp byte [scratch+offset-1],0x69
    jne failed
    cmp byte [scratch+offset+4],0x96
    jne failed
    inc dword [checks]
%assign offset offset+1
%endrep

    mov eax,[left]
    FLAGS_START
    xchg %1,%1
    FLAGS_CHECK
    cmp eax,[left]
    jne failed
    inc dword [checks]
%endmacro

start:
    push cs
    pop ds
    cld
    pushfd
    pop eax
    and eax,~0x0cd5
    mov [flag_base],eax
    mov byte [round],2
.round:
    xor edi,edi
.value:
    mov eax,edi
    and eax,0x0cd5
    or eax,[flag_base]
    mov [flags],eax
    mov eax,edi
    or eax,[upper]
    mov [left],eax
    not eax
    mov [right],eax
    NOT_WIDTH al,byte,0xff
    NOT_WIDTH ax,word,0xffff
    NOT_WIDTH eax,dword,0xffffffff
    EXCHANGE_WIDTH al,bl,byte,0xff
    EXCHANGE_WIDTH ax,bx,word,0xffff
    EXCHANGE_WIDTH eax,ebx,dword,0xffffffff

    mov byte [phase],3
    mov ebx,[left]
    mov esi,edi
    xor esi,0x1234
    or esi,0x5aa50000
    mov [lea_index],esi
    mov ebp,ebx
    add ebp,esi
    add ebp,123
    and ebp,0xffff
    mov eax,[left]
    and eax,0xffff0000
    or ebp,eax
    FLAGS_START
    lea ax,[bx+si+123]
    FLAGS_CHECK
    cmp eax,ebp
    jne failed
    inc dword [checks]

    mov ebp,ebx
    add ebp,esi
    add ebp,123
    and ebp,0xffff
    mov eax,[left]
    FLAGS_START
    lea eax,[bx+si+123]
    FLAGS_CHECK
    cmp eax,ebp
    jne failed
    inc dword [checks]

    mov ebp,esi
    shl ebp,2
    add ebp,ebx
    add ebp,0x12345678
    mov [address32],ebp
    and ebp,0xffff
    mov eax,[left]
    and eax,0xffff0000
    or ebp,eax
    FLAGS_START
    lea ax,[ebx+esi*4+0x12345678]
    FLAGS_CHECK
    cmp eax,ebp
    jne failed
    inc dword [checks]

    mov eax,[left]
    FLAGS_START
    lea eax,[ebx+esi*4+0x12345678]
    FLAGS_CHECK
    cmp eax,[address32]
    jne failed
    cmp ebx,[left]
    jne failed
    cmp esi,[lea_index]
    jne failed
    inc dword [checks]

    inc edi
    cmp edi,65536
    jb .value
    xor dword [upper],0xffff0000
    dec byte [round]
    jnz .round
    cld
    cmp dword [checks],3276800
    jne failed
    mov dx,success
    mov ah,9
    int 0x21
    mov ax,0x4c00
    int 0x21
failed:
    cld
    mov dx,failure
    mov ah,9
    int 0x21
    mov dl,[phase]
    add dl,'0'
    mov ah,2
    int 0x21
    mov ax,0x4c01
    int 0x21

round db 0
phase db 0
upper dd 0xa55a0000
flag_base dd 0
flags dd 0
left dd 0
right dd 0
expected_left dd 0
expected_right dd 0
address32 dd 0
lea_index dd 0
checks dd 0
guard db 0
align 4,db 0
scratch times 6 db 0
success db 'CCPU NOT XCHG LEA PASS checks=3276800',13,10,'$'
failure db 'CCPU TRANSFER FAIL phase=$'
