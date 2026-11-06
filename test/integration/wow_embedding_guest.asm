; Independently authored Win16 embedding enumeration witness.
; Uses original public KERNEL/USER/GDI ordinals; no guest mirror modifications.
bits 16
org 0
db 'MZ'
dw (image_end-$$) % 512,((image_end-$$)+511)/512,0,4,0,0xffff,0,0x100,0,0,0,0x40,0
times 0x3c-($-$$) db 0
dd ne
ne:
db 'NE',5,1
dw entries-ne,entries_end-entries
dd 0
dw 0x0302,2,0x100,0x1000
dw 0,1,0,2
dw 2,3,0
dw segments-ne,resources-ne,resident-ne,modules-ne,imports-ne
dd 0
dw 0,4,0
db 2,0
dw 0,0,0,0x030a
segments:
dw (code-$$)/16,code_end-code,0x0150,code_end-code
dw (data-$$)/16,data_end-data,0x0c51,0x4000
resources: dw 4,0
resident: db 8,'EMBTEST1'
dw 0
db 0
modules: dw kernel-imports,user-imports,gdi-imports
imports:
db 0
kernel: db 6,'KERNEL'
user: db 4,'USER'
gdi: db 5,'SHELL'
entries: db 0
entries_end:
%assign calls 0
%macro api 2
%assign calls calls+1
..@call_%[calls]: db 0x9a
dw 0xffff,0
%xdefine module_%[calls] %1
%xdefine ordinal_%[calls] %2
%endmacro

align 16,db 0
code:
api 1,91
or ax,ax
jz failed
push di
api 2,5
or ax,ax
jz failed
push ds
push word filename-data
push word 0
api 1,83
mov [file-data],ax
; Let the original two-second embedding freshness window expire.
api 2,13
mov [start-data],ax
.wait:
api 1,29
api 2,13
sub ax,[start-data]
cmp ax,2100
jb .wait
mov word [phase-data],1
call report_now
push ds
push word profile_section-data
push word 0
push word 0
push ds
push word empty-data
push ds
push word buffer-data
push word 256
api 1,58
mov [result-data],ax
cmp ax,255
ja failed
cmp word [guard-data],0xa55a
jne failed
; Only shadow Registry calls follow; never query embedding after adding keys.
push word 0x8000
push word 0
push ds
push word key_name-data
push ds
push word key_handle-data
mov word [reserved-data],10
api 3,2
mov [result-data],ax
or ax,dx
jnz failed
cmp dword [key_handle-data],1
je failed
push word [key_handle-data+2]
push word [key_handle-data]
push word 0
push word 0
push word 0
push word 1
push ds
push word value-data
push word 0
push word 0
mov word [reserved-data],20
api 3,5
mov [result-data],ax
or ax,dx
jnz failed
push word [key_handle-data+2]
push word [key_handle-data]
push word 0
push word 0
push ds
push word registry_buffer-data
push ds
push word registry_size-data
mov word [reserved-data],30
api 3,6
mov [result-data],ax
or ax,dx
jnz failed
cmp word [registry_size-data],6
jne failed
cmp word [registry_size-data+2],0
jne failed
cmp byte [registry_buffer-data+5],0
jne failed
mov si,value-data
mov di,registry_buffer-data
mov cx,6
push ds
pop es
repe cmpsb
jne failed
push word [key_handle-data+2]
push word [key_handle-data]
mov word [reserved-data],40
api 3,3
mov [result-data],ax
or ax,dx
jnz failed
mov word [key_handle-data],0
mov word [key_handle-data+2],0
push word 0x8000
push word 0
push ds
push word key_name-data
mov word [reserved-data],50
api 3,4
mov [result-data],ax
or ax,dx
jnz failed
push word 0x8000
push word 0
push ds
push word direct_name-data
push word 0
push word 1
push ds
push word value-data
push word 0
push word 0
mov word [reserved-data],60
api 3,5
mov [result-data],ax
or ax,dx
jnz failed
mov dword [registry_size-data],32
push word 0x8000
push word 0
push ds
push word direct_name-data
push ds
push word registry_buffer-data
push ds
push word registry_size-data
mov word [reserved-data],70
api 3,6
mov [result-data],ax
or ax,dx
jnz failed
cmp dword [registry_size-data],6
jne failed
cmp byte [registry_buffer-data+5],0
jne failed
push word 0x8000
push word 0
push ds
push word direct_name-data
mov word [reserved-data],80
api 3,4
mov [result-data],ax
or ax,dx
jnz failed
mov word [reserved-data],6
mov word [phase-data],2
jmp finish
failed:
mov ax,[reserved-data]
or ax,ax
jnz .have_step
mov ax,1
.have_step:
mov [errors-data],ax
finish:
call report_now
push word [file-data]
api 1,81
mov ax,0x4c00
or al,[errors-data]
int 0x21
report_now:
push word [file-data]
push ds
push word report-data
push word report_end-report
api 1,86
ret
code_end:
dw calls
%assign call_number 1
%rep calls
db 3,1
dw ..@call_%[call_number]-code+1,module_%[call_number],ordinal_%[call_number]
%assign call_number call_number+1
%endrep
align 16,db 0
data:
times 32 db 0
report: db 'EMB1'
phase: dw 0
errors: dw 0
result: dw 0
reserved: dw 0
report_end:
file: dw -1
start: dw 0
key_handle: dd 0
registry_size: dd 32
key_name: db 'T478S24Fixture',0
direct_name: db 'T478S24Direct',0
value: db 'Value',0
registry_buffer: times 32 db 0xcc
filename: db 'EMBTEST.BIN',0
profile_section: db 'embedding',0
empty: db 0
buffer: times 256 db 0
guard: dw 0xa55a
data_end:
align 16,db 0
image_end:
