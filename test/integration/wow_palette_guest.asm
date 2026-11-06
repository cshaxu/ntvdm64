; Independently authored Win16 palette returned-count witness.
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
resident: db 8,'PALTEST1'
dw 0
db 0
modules: dw kernel-imports,user-imports,gdi-imports
imports:
db 0
kernel: db 6,'KERNEL'
user: db 4,'USER'
gdi: db 3,'GDI'
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
push ds
push word palette-data
api 3,360
mov [handle-data],ax
or ax,ax
jz failed
call fill
push word [handle-data]
push word 1
push word 4
push ds
push word buffer-data
api 3,363
mov [returned-data],ax
cmp ax,1
jne failed
cmp byte [buffer-data],4
jne failed
mov si,buffer-data+4
mov cx,12
call tail
; Exact two-entry query must still preserve its valid RGB output.
call fill
push word [handle-data]
push word 0
push word 2
push ds
push word buffer-data
api 3,363
cmp ax,2
jne failed
cmp byte [buffer-data],1
jne failed
cmp byte [buffer-data+4],4
jne failed
mov si,buffer-data+8
mov cx,8
call tail
; A valid palette with start at its end returns no entries.
call fill
push word [handle-data]
push word 2
push word 4
push ds
push word buffer-data
api 3,363
or ax,ax
jnz failed
mov si,buffer-data
mov cx,16
call tail
push word [handle-data]
api 3,69
or ax,ax
jz failed
mov word [handle-data],0
mov word [errors-data],0
jmp finish
failed:
mov word [errors-data],1
finish:
cmp word [handle-data],0
je .write
push word [handle-data]
api 3,69
.write:
push word [file-data]
push ds
push word report-data
push word report_end-report
api 1,86
push word [file-data]
api 1,81
mov ax,0x4c00
or al,[errors-data]
int 0x21
fill:
push ds
pop es
mov di,buffer-data
mov cx,16
mov al,0xa5
rep stosb
ret
tail:
cmp byte [si],0xa5
jne failed
inc si
loop tail
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
report: db 'PAL1'
errors: dw 1
count: dw 1
requested: dw 4
returned: dw 0
report_end:
file: dw -1
handle: dw 0
filename: db 'PALTEST.BIN',0
palette: dw 0x300,2
db 1,2,3,0,4,5,6,0
buffer: times 16 db 0
data_end:
align 16,db 0
image_end:
