; Independently authored Win16 kerning returned-count witness.
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
resident: db 8,'KERNTEST'
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
push word 0
api 3,52
mov [dc-data],ax
or ax,ax
jz failed
push ds
push word lf-data
api 3,57
mov [font-data],ax
or ax,ax
jz failed
push word [dc-data]
push ax
api 3,45
mov [old-data],ax
push word [dc-data]
push word 0
push word 0
push word 0
api 3,332
mov [count-data],ax
or ax,ax
jz failed
cmp ax,500
ja failed
add ax,3
mov [requested-data],ax
push ds
pop es
mov di,buffer-data
mov cx,3072
mov al,0xa5
rep stosb
push word [dc-data]
push word [requested-data]
push ds
push word buffer-data
api 3,332
mov [returned-data],ax
cmp ax,[count-data]
jne failed
; Original KERNINGPAIR16 consists of three WORDs.
mov bx,6
mul bx
mov si,buffer-data
add si,ax
mov cx,18
.tail:
cmp byte [si],0xa5
jne failed
inc si
loop .tail
mov word [errors-data],0
jmp finish
failed:
mov word [errors-data],1
finish:
push word [dc-data]
push word [old-data]
api 3,45
push word [font-data]
api 3,69
push word [dc-data]
api 3,68
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
report: db 'KRN1'
errors: dw 1
count: dw 0
requested: dw 0
returned: dw 0
report_end:
file: dw -1
dc: dw 0
font: dw 0
old: dw 0
filename: db 'KERNTEST.BIN',0
lf: dw 24,0,0,0,400
db 0,0,0,0,7,0,0,0
db 'Arial',0
times 26 db 0
buffer: times 3072 db 0
data_end:
align 16,db 0
image_end:
