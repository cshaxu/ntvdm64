; Independently authored test input, never a guest system replacement.
; Adapted from independently authored win16_dc_probe.asm: direct caret thunk witness.
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
dw (data-$$)/16,data_end-data,0x0c51,0x8000
resources: dw 0,0
resident: db 7,'DCPROBE'
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
%macro check 1
test ax,ax
jnz %%ok
mov word [status-data],%1
jmp report_result
%%ok:
%endmacro
%macro release_dc 0
push word [window-data]
push word [dc-data]
api 2,68
check 8
%endmacro
align 16,db 0
code:
api 1,91
check 1
mov [instance-data],di
push di
api 2,5
check 2
 ; Original DPMIBOP GetFastBopAddress (BOP 53, subfunction 3).
push es
 db 0xc4,0xc4,0x53,0x03
mov [endpoint-data],es
mov [offset-data],bx
mov [offsethi-data],dx
pop es
cmp word [endpoint-data],0
je no_endpoint
; Exhaust at most 8190 user selectors, immediately retaining each allocated
; identity and testing whether original KERNEL AllocSelector publishes endpoint.
alloc_next:
cmp word [count-data],8190
jae release_all
push word 0
api 1,175 ; AllocSelector(invalid selector): fresh descriptor
or ax,ax
jz release_all
mov bx,[count-data]
shl bx,1
mov [allocations+bx-data],ax
inc word [count-data]
mov bx,[count-data]
mov [peak-data],bx
cmp ax,[endpoint-data]
jne alloc_next
mov word [hit-data],1
release_all:
cmp word [count-data],0
je report_result
dec word [count-data]
mov bx,[count-data]
shl bx,1
push word [allocations+bx-data]
api 1,176 ; FreeSelector
jmp release_all
no_endpoint:
mov word [status-data],1
report_result:
push ds
push word filename-data
push word 0
api 1,83
cmp ax,-1
je fatal
mov [file-data],ax
push ax
push ds
push word report-data
push word report_end-report
api 1,86
cmp ax,report_end-report
jne fatal
push word [file-data]
api 1,81
cmp word [status-data],0
jne fatal
mov ax,0x4c00
int 0x21
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
report: db 'FB01'
endpoint: dw 0
offset: dw 0
offsethi: dw 0
hit: dw 0
status: dw 0
count: dw 0
peak: dw 0
report_end:
instance: dw 0
window: dw 0
dc: dw 0
file: dw -1
class_name: db 'STATIC',0
window_name: db 'DC',0
filename: db 'DCPROBE.BIN',0
allocations: times 8190 dw 0
property_name: db 'DCPROP',0
class_buffer: times 16 db 0
rect: times 8 db 0
data_end:
align 16,db 0
image_end:
