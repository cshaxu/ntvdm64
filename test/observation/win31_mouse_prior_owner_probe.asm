; Isolated Win16 test: preserve a prior protected mouse callback across
; the authored mouse driver's Disable/Enable/Disable sequence.
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
resident: db 7,'M31OWNR'
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
mov [module-data],ax
push ax
push word 0
push word 2
api 1,50
mov [enable-data],ax
mov [enable+2-data],dx
mov bx,ax
mov es,dx
cmp byte [es:bx],0xb8
jne failed
mov ax,[es:bx+1]
mov es,ax
%ifdef MOUSE31_OWNER_MODE_TEST
cmp byte [es:52],1
%elifdef MOUSE31_OWNER_ORIGINAL
cmp byte [es:46],1
%else
cmp byte [es:42],1
%endif
jne failed
mov ax,[es:30]
mov [event-data],ax
mov ax,[es:32]
mov [event+2-data],ax
push word [module-data]
push word 0
push word 3
api 1,50
mov [disable-data],ax
mov [disable+2-data],dx
; Patch the fixed callback's own DS through a temporary writable alias.
push cs
pop bx
mov ax,0x000a
int 0x31
jc failed
mov es,ax
mov [es:prior_ds-code],ds
mov bx,ax
mov ax,0x0001
int 0x31
jc failed
; Shut down the Windows bridge, establish an earlier PM owner, then cycle it.
call far [disable-data]
push cs
pop es
mov dx,prior-code
mov ax,0x000c
mov cx,0x001f
int 0x33
%ifndef MOUSE31_OWNER_CONTROL
push word [event+2-data]
push word [event-data]
call far [enable-data]
test ax,ax
jz failed
call far [disable-data]
%endif
mov word [phase-data],1
call checkpoint
; Wait up to 15 seconds, yielding so the private input probe can deliver a
; real mouse event. Tick arithmetic uses the low word and a bounded interval.
api 2,13
mov [start-data],ax
wait_input:
cmp word [observed-data],0
jne received
api 1,29
api 2,13
sub ax,[start-data]
cmp ax,15000
jb wait_input
received:
mov word [phase-data],2
call checkpoint
; Restore the Windows bridge before this test program unloads.
push word [event+2-data]
push word [event-data]
call far [enable-data]
push word [file-data]
api 1,81
mov ax,0x4c00
int 0x21
prior:
pushf
pusha
push ds
db 0xb8
prior_ds: dw 0
mov ds,ax
inc word [observed-data]
pop ds
popa
popf
retf
checkpoint:
push word [file-data]
push ds
push word result-data
push word result_end-result
api 1,86
cmp ax,result_end-result
jne fatal
ret
failed:
inc word [errors-data]
call checkpoint
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
result: db 'M31O'
phase: dw 0
errors: dw 0
observed: dw 0
result_end:
file: dw 0
instance: dw 0
module: dw 0
start: dw 0
enable: dd 0
disable: dd 0
event: dd 0
module_name: db 'MOUSE',0
filename: db 'M31OWNR.BIN',0
data_end:
image_end:
