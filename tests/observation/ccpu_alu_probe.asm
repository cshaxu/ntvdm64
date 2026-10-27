; Independent ALU reference-data guest. No original guest is modified.
bits 16
org 100h
jmp start

%macro PREPARE 2
    mov eax,[si]
    mov ebx,[si+4]
    mov ecx,[si+8]
%if %1 == 8
    or eax,0xa55ac300
    or ebx,0x5aa53c00
    or ecx,0xa55ac300
%elif %1 == 16
    or eax,0xa55a0000
    or ebx,0x5aa50000
    or ecx,0xa55a0000
%endif
    mov [expected],ecx
    mov [lhs_saved],eax
    mov [rhs_saved],ebx
%endmacro

%macro INSTRUCTION 5
; mnemonic,width,form,unary,incoming CF
%if %5
    stc
%else
    clc
%endif
%if %2 == 8
%if %3 == 1
%if %4
    %1 al
%else
    %1 al,bl
%endif
%elif %3 == 2
%if %4
    %1 byte [scratch]
%else
    %1 byte [scratch],bl
%endif
%else
%if %4
    %1 byte [scratch+1]
%else
    %1 al,byte [scratch]
%endif
%endif
%elif %2 == 16
%if %3 == 1
%if %4
    %1 ax
%else
    %1 ax,bx
%endif
%elif %3 == 2
%if %4
    %1 word [scratch]
%else
    %1 word [scratch],bx
%endif
%else
%if %4
    %1 word [scratch+1]
%else
    %1 ax,word [scratch]
%endif
%endif
%else
%if %3 == 1
%if %4
    %1 eax
%else
    %1 eax,ebx
%endif
%elif %3 == 2
%if %4
    %1 dword [scratch]
%else
    %1 dword [scratch],ebx
%endif
%else
%if %4
    %1 dword [scratch+1]
%else
    %1 eax,dword [scratch]
%endif
%endif
%endif
    pushfd
    pop edx
%endmacro

%macro RUN 8
; phase,filename,marker,width,mnemonic,carry,unary,records
    mov byte [phase],%1
    mov byte [reason],'O'
    mov dx,%2
    mov ax,0x3d00
    int 0x21
    jc failed
    mov [handle],ax
    mov dword [remaining],%8
    mov dword [ordinal],0
%%read:
    mov byte [reason],'R'
    mov bx,[handle]
    mov cx,14336
    mov dx,buffer
    mov ah,0x3f
    int 0x21
    jc failed
    test ax,ax
    jz failed
    mov bp,ax
    xor dx,dx
    mov cx,14
    div cx
    test dx,dx
    jnz failed
    mov si,buffer
%%record:
    mov byte [form],1
    PREPARE %4,%1
    INSTRUCTION %5,%4,1,%7,%6
    call check

    mov byte [form],2
    PREPARE %4,%1
    mov [scratch],eax
    mov byte [scratch-1],0x69
    mov byte [scratch+4],0x96
    INSTRUCTION %5,%4,2,%7,%6
    mov byte [reason],'G'
    cmp eax,[lhs_saved]
    jne failed
    cmp byte [scratch-1],0x69
    jne failed
    cmp byte [scratch+4],0x96
    jne failed
    mov eax,[scratch]
    call check

    mov byte [form],3
    PREPARE %4,%1
%if %7
    mov [scratch+1],eax
    mov byte [scratch],0x69
    mov byte [scratch+5],0x96
%else
    mov [scratch],ebx
%endif
    INSTRUCTION %5,%4,3,%7,%6
    mov byte [reason],'G'
%if %7
    cmp eax,[lhs_saved]
    jne failed
    cmp byte [scratch],0x69
    jne failed
    cmp byte [scratch+5],0x96
    jne failed
    mov eax,[scratch+1]
%else
    mov ecx,[scratch]
    cmp ecx,[rhs_saved]
    jne failed
%endif
    call check
    inc dword [total_records]
    inc dword [ordinal]
    dec dword [remaining]
    add si,14
    sub bp,14
    cmp dword [remaining],0
    je %%done
    test bp,bp
    jnz %%record
    jmp %%read
%%done:
    mov byte [reason],'E'
    test bp,bp
    jnz failed
    mov bx,[handle]
    mov cx,1
    mov dx,eof_byte
    mov ah,0x3f
    int 0x21
    jc failed
    test ax,ax
    jnz failed
    mov ah,0x3e
    int 0x21
    jc failed
    inc byte [total_phases]
    mov dx,%3
    call print
%endmacro

start:
    push cs
    pop ds
    push cs
    pop es
    cld
    mov byte [reason],'A'
    mov ax,scratch
    test ax,3
    jnz failed
    RUN 1,f1,m1,8,add,0,0,65536
    RUN 2,f2,m2,16,add,0,0,1196
    RUN 3,f3,m3,32,add,0,0,1196
    RUN 4,f4,m4,8,adc,0,0,65536
    RUN 5,f5,m5,16,adc,0,0,1196
    RUN 6,f6,m6,32,adc,0,0,1196
    RUN 7,f7,m7,8,adc,1,0,65536
    RUN 8,f8,m8,16,adc,1,0,1196
    RUN 9,f9,m9,32,adc,1,0,1196
    RUN 10,f10,m10,8,and,0,0,65536
    RUN 11,f11,m11,16,and,0,0,1196
    RUN 12,f12,m12,32,and,0,0,1196
    RUN 13,f13,m13,8,cmp,0,0,65536
    RUN 14,f14,m14,16,cmp,0,0,1196
    RUN 15,f15,m15,32,cmp,0,0,1196
    RUN 16,f16,m16,8,dec,0,1,65536
    RUN 17,f17,m17,16,dec,0,1,1196
    RUN 18,f18,m18,32,dec,0,1,1196
    RUN 19,f19,m19,8,dec,1,1,65536
    RUN 20,f20,m20,16,dec,1,1,1196
    RUN 21,f21,m21,32,dec,1,1,1196
    RUN 22,f22,m22,8,inc,0,1,65536
    RUN 23,f23,m23,16,inc,0,1,1196
    RUN 24,f24,m24,32,inc,0,1,1196
    RUN 25,f25,m25,8,inc,1,1,65536
    RUN 26,f26,m26,16,inc,1,1,1196
    RUN 27,f27,m27,32,inc,1,1,1196
    RUN 28,f28,m28,8,neg,0,1,65536
    RUN 29,f29,m29,16,neg,0,1,1196
    RUN 30,f30,m30,32,neg,0,1,1196
    RUN 31,f31,m31,8,or,0,0,65536
    RUN 32,f32,m32,16,or,0,0,1196
    RUN 33,f33,m33,32,or,0,0,1196
    RUN 34,f34,m34,8,sbb,0,0,65536
    RUN 35,f35,m35,16,sbb,0,0,1196
    RUN 36,f36,m36,32,sbb,0,0,1196
    RUN 37,f37,m37,8,sbb,1,0,65536
    RUN 38,f38,m38,16,sbb,1,0,1196
    RUN 39,f39,m39,32,sbb,1,0,1196
    RUN 40,f40,m40,8,sub,0,0,65536
    RUN 41,f41,m41,16,sub,0,0,1196
    RUN 42,f42,m42,32,sub,0,0,1196
    RUN 43,f43,m43,8,test,0,0,65536
    RUN 44,f44,m44,16,test,0,0,1196
    RUN 45,f45,m45,32,test,0,0,1196
    RUN 46,f46,m46,8,xor,0,0,65536
    RUN 47,f47,m47,16,xor,0,0,1196
    RUN 48,f48,m48,32,xor,0,0,1196
    mov byte [reason],'C'
    cmp dword [total_records],1086848
    jne failed
    cmp dword [total_ops],3260544
    jne failed
    cmp byte [total_phases],48
    jne failed
    mov dx,success
    call print
    mov ax,0x4c00
    int 0x21

check:
    mov byte [reason],'V'
    mov [actual],eax
    mov [actual_flags],edx
    cmp eax,[expected]
    jne failed
    cmp ebx,[rhs_saved]
    jne failed
    mov byte [reason],'F'
    xor dx,[si+12]
    and dx,0x08d5
    jnz failed
    inc dword [total_ops]
    ret
print:
    mov ah,9
    int 0x21
    ret
hex:
    push ax
    mov dl,al
    shr dl,4
    call digit
    pop ax
    mov dl,al
    and dl,15
    call digit
    ret
digit:
    add dl,'0'
    cmp dl,'9'
    jbe .decimal
    add dl,7
.decimal:
    mov ah,2
    int 0x21
    ret
failed:
    cld
    mov dx,failure
    call print
    mov al,[phase]
    call hex
    mov dx,form_label
    call print
    mov al,[form]
    call hex
    mov dx,ordinal_label
    call print
    mov al,[ordinal+1]
    call hex
    mov al,[ordinal]
    call hex
    mov dx,reason_label
    call print
    mov dl,[reason]
    mov ah,2
    int 0x21
    mov ax,0x4c01
    int 0x21

handle dw 0
phase db 0
form db 0
reason db 0
total_phases db 0
eof_byte db 0
remaining dd 0
ordinal dd 0
total_records dd 0
total_ops dd 0
expected dd 0
lhs_saved dd 0
rhs_saved dd 0
actual dd 0
actual_flags dd 0
guard db 0
align 4,db 0
scratch times 6 db 0
success db 'CCPU ALU PASS phases=48 records=1086848 instructions=3260544',13,10,'$'
failure db 'CCPU ALU FAIL phase=$'
form_label db ' form=$'
ordinal_label db ' index=$'
reason_label db ' reason=$'
f1 db 'ADD8.BIN',0
m1 db 'ADD8.BIN PASS',13,10,'$'
f2 db 'ADD16.BIN',0
m2 db 'ADD16.BIN PASS',13,10,'$'
f3 db 'ADD32.BIN',0
m3 db 'ADD32.BIN PASS',13,10,'$'
f4 db 'ADC0_8.BIN',0
m4 db 'ADC0_8.BIN PASS',13,10,'$'
f5 db 'ADC0_16.BIN',0
m5 db 'ADC0_16.BIN PASS',13,10,'$'
f6 db 'ADC0_32.BIN',0
m6 db 'ADC0_32.BIN PASS',13,10,'$'
f7 db 'ADC1_8.BIN',0
m7 db 'ADC1_8.BIN PASS',13,10,'$'
f8 db 'ADC1_16.BIN',0
m8 db 'ADC1_16.BIN PASS',13,10,'$'
f9 db 'ADC1_32.BIN',0
m9 db 'ADC1_32.BIN PASS',13,10,'$'
f10 db 'AND8.BIN',0
m10 db 'AND8.BIN PASS',13,10,'$'
f11 db 'AND16.BIN',0
m11 db 'AND16.BIN PASS',13,10,'$'
f12 db 'AND32.BIN',0
m12 db 'AND32.BIN PASS',13,10,'$'
f13 db 'CMP8.BIN',0
m13 db 'CMP8.BIN PASS',13,10,'$'
f14 db 'CMP16.BIN',0
m14 db 'CMP16.BIN PASS',13,10,'$'
f15 db 'CMP32.BIN',0
m15 db 'CMP32.BIN PASS',13,10,'$'
f16 db 'DEC0_8.BIN',0
m16 db 'DEC0_8.BIN PASS',13,10,'$'
f17 db 'DEC0_16.BIN',0
m17 db 'DEC0_16.BIN PASS',13,10,'$'
f18 db 'DEC0_32.BIN',0
m18 db 'DEC0_32.BIN PASS',13,10,'$'
f19 db 'DEC1_8.BIN',0
m19 db 'DEC1_8.BIN PASS',13,10,'$'
f20 db 'DEC1_16.BIN',0
m20 db 'DEC1_16.BIN PASS',13,10,'$'
f21 db 'DEC1_32.BIN',0
m21 db 'DEC1_32.BIN PASS',13,10,'$'
f22 db 'INC0_8.BIN',0
m22 db 'INC0_8.BIN PASS',13,10,'$'
f23 db 'INC0_16.BIN',0
m23 db 'INC0_16.BIN PASS',13,10,'$'
f24 db 'INC0_32.BIN',0
m24 db 'INC0_32.BIN PASS',13,10,'$'
f25 db 'INC1_8.BIN',0
m25 db 'INC1_8.BIN PASS',13,10,'$'
f26 db 'INC1_16.BIN',0
m26 db 'INC1_16.BIN PASS',13,10,'$'
f27 db 'INC1_32.BIN',0
m27 db 'INC1_32.BIN PASS',13,10,'$'
f28 db 'NEG8.BIN',0
m28 db 'NEG8.BIN PASS',13,10,'$'
f29 db 'NEG16.BIN',0
m29 db 'NEG16.BIN PASS',13,10,'$'
f30 db 'NEG32.BIN',0
m30 db 'NEG32.BIN PASS',13,10,'$'
f31 db 'OR8.BIN',0
m31 db 'OR8.BIN PASS',13,10,'$'
f32 db 'OR16.BIN',0
m32 db 'OR16.BIN PASS',13,10,'$'
f33 db 'OR32.BIN',0
m33 db 'OR32.BIN PASS',13,10,'$'
f34 db 'SBB0_8.BIN',0
m34 db 'SBB0_8.BIN PASS',13,10,'$'
f35 db 'SBB0_16.BIN',0
m35 db 'SBB0_16.BIN PASS',13,10,'$'
f36 db 'SBB0_32.BIN',0
m36 db 'SBB0_32.BIN PASS',13,10,'$'
f37 db 'SBB1_8.BIN',0
m37 db 'SBB1_8.BIN PASS',13,10,'$'
f38 db 'SBB1_16.BIN',0
m38 db 'SBB1_16.BIN PASS',13,10,'$'
f39 db 'SBB1_32.BIN',0
m39 db 'SBB1_32.BIN PASS',13,10,'$'
f40 db 'SUB8.BIN',0
m40 db 'SUB8.BIN PASS',13,10,'$'
f41 db 'SUB16.BIN',0
m41 db 'SUB16.BIN PASS',13,10,'$'
f42 db 'SUB32.BIN',0
m42 db 'SUB32.BIN PASS',13,10,'$'
f43 db 'TEST8.BIN',0
m43 db 'TEST8.BIN PASS',13,10,'$'
f44 db 'TEST16.BIN',0
m44 db 'TEST16.BIN PASS',13,10,'$'
f45 db 'TEST32.BIN',0
m45 db 'TEST32.BIN PASS',13,10,'$'
f46 db 'XOR8.BIN',0
m46 db 'XOR8.BIN PASS',13,10,'$'
f47 db 'XOR16.BIN',0
m47 db 'XOR16.BIN PASS',13,10,'$'
f48 db 'XOR32.BIN',0
m48 db 'XOR32.BIN PASS',13,10,'$'
buffer: times 14336 db 0
