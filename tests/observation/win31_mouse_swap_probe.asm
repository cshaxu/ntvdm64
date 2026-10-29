; Test-only Win16 program: inspect protected INT33/14 exchange; no guest patch.
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
dw 2,2,0
dw segments-ne,resources-ne,resident-ne,modules-ne,imports-ne
dd 0
dw 0,4,0
db 2,0
dw 0,0,0,0x030a
segments:
dw (code-$$)/16,code_end-code,0x0150,code_end-code
dw (data-$$)/16,data_end-data,0x0c51,0x4000
resources: dw 0,0
resident: db 7,'M31SWAP'
dw 0
db 0
modules: dw kernel-imports,user-imports
imports:
db 0
kernel: db 6,'KERNEL'
user: db 4,'USER'
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
test ax,ax
jz fatal
mov [instance-data],di
push word [instance-data]
api 2,5
test ax,ax
jz fatal
push ds
push word filename-data
push word 0
api 1,83
cmp ax,-1
je fatal
mov [file-data],ax
push ds
push word module_name-data
api 1,47
test ax,ax
jz failed
push ax
push word 0
push word 2
api 1,50
mov bx,ax
mov es,dx
cmp byte [es:bx],0xb8
jne failed
mov ax,[es:bx+1]
mov es,ax
cmp byte [es:42],1
jne failed
mov ax,[es:46]
mov [thunk-data],ax
mov ax,[es:48]
mov [thunk+2-data],ax
push cs
pop es
mov dx,prior-code
mov [expected_off-data],dx
mov [expected_seg-data],es
mov ax,0x000c
mov cx,0x001f
int 0x33
xor ax,ax
mov es,ax
xor dx,dx
xor cx,cx
mov ax,0x0014
int 0x33
mov [returned_off-data],dx
mov [returned_seg-data],es
mov [returned_mask-data],cx
; Always restore our driver's protected registration before writing results.
les dx,[thunk-data]
mov cx,0x001f
mov ax,0x000c
int 0x33
push word [file-data]
push ds
push word result-data
push word result_end-result
api 1,86
cmp ax,result_end-result
jne fatal
push word [file-data]
api 1,81
mov ax,0x4c00
int 0x21
prior: retf
failed:
push word [file-data]
api 1,81
fatal:
mov ax,0x4c01
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
result: db 'M31S'
expected_off: dw 0
expected_seg: dw 0
returned_off: dw 0
returned_seg: dw 0
returned_mask: dw 0
result_end:
file: dw 0
instance: dw 0
thunk: dd 0
module_name: db 'MOUSE',0
filename: db 'M31SWAP.BIN',0
data_end:
image_end:
