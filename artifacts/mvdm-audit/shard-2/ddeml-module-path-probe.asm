; Independently authored Win16 NE diagnostic, not a guest replacement.
; Reports LoadLibrary/real module pathname and DDEINITIALIZE export only.
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
dw 2,1,0
dw segments-ne,resources-ne,resident-ne,modules-ne,imports-ne
dd 0
dw 0,4,0
db 2,0
dw 0,0,0,0x030a
segments:
dw (code-$$)/16,code_end-code,0x0150,code_end-code
dw (data-$$)/16,data_end-data,0x0c51,0x4000
resources: dw 0,0
resident: db 7,'DDPATH1'
dw 0
db 0
modules: dw kernel-imports
imports:
db 0
kernel: db 6,'KERNEL'
entries: db 0
entries_end:
%macro api 1
..@call_%1: db 0x9a
dw 0xffff,0
%endmacro
align 16,db 0
code:
api 1
 test ax,ax
 jz fatal
 push di
 api 2
 test ax,ax
 jz fatal
%ifdef PROBE_NO_DLL
 jmp report
%endif
 push word 0x8000
 api 3
 push ds
 push word dllname-data
 api 4
 mov [module_handle-data],ax
 cmp ax,32
 jb report
 push ax
 push ds
 push word module_path-data
 push word 260
 api 5
 mov [path_length-data],ax
 push word [module_handle-data]
 push word 0
 push word 2
 api 6
 mov [entry_offset-data],ax
 mov [entry_selector-data],dx
 push word [module_handle-data]
 api 7
report:
 push ds
 push word result_name-data
 push word 0
 api 8
 cmp ax,-1
 je fatal
 mov [file-data],ax
 push ax
 push ds
 push word report_start-data
 push word report_end-report_start
 api 9
 cmp ax,report_end-report_start
 jne fatal
 push word [file-data]
 api 10
 mov ax,0x4c00
 int 0x21
fatal:
 mov ax,0x4c01
 int 0x21
code_end:
dw 10
%macro fixup 2
db 3,1
dw ..@call_%1-code+1,1,%2
%endmacro
fixup 1,91
fixup 2,5
fixup 3,107
fixup 4,95
fixup 5,49
fixup 6,50
fixup 7,96
fixup 8,83
fixup 9,86
fixup 10,81
align 16,db 0
data:
times 32 db 0
report_start: db 'DDP1'
module_handle: dw 0
path_length: dw 0
entry_offset: dw 0
entry_selector: dw 0
module_path: times 260 db 0
report_end:
file: dw 0
dllname: db 'DDEML.DLL',0
result_name: db 'DDPATH.BIN',0
data_end:
image_end:
