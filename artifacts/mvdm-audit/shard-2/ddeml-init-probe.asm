; Independently authored Win16 probe. Does not modify original guest code.
; Static DDEML import; initializes, reinitializes, and uninitializes one instance.
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
resident: db 7,'DDINIT1'
dw 0
db 0
modules: dw kernel-imports,ddeml-imports
imports:
db 0
kernel: db 6,'KERNEL'
ddeml: db 5,'DDEML'
entries: db 0
entries_end:
%macro api 1
..@call_%1: db 0x9a
dw 0xffff,0
%endmacro
%macro dde_args 1
 push ds
 push word instance-data
 push cs
 push word dde_callback-code
 push word 0
 push word 0x0010 ; APPCMD_CLIENTONLY
 push word 0
 push word %1
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
 push ds
 push word module_name-data
 api 3
 mov [module_handle-data],ax
 test ax,ax
 jz report
 push ax
 push ds
 push word module_path-data
 push word 260
 api 4
 mov [path_length-data],ax
 dde_args 1
 api 5
 mov [invalid_result-data],ax
 dde_args 0
 api 6
 mov [init_result-data],ax
 mov ax,[instance-data]
 mov [init_id_lo-data],ax
 mov ax,[instance+2-data]
 mov [init_id_hi-data],ax
 mov ax,[init_result-data]
 test ax,ax
 jnz report
 mov ax,[instance-data]
 or ax,[instance+2-data]
 jz report
 dde_args 0
 api 7
 mov [reinit_result-data],ax
 mov ax,[instance-data]
 mov [reinit_id_lo-data],ax
 mov ax,[instance+2-data]
 mov [reinit_id_hi-data],ax
 push word [instance+2-data]
 push word [instance-data]
 api 8
 mov [uninit_result-data],ax
 push word [instance+2-data]
 push word [instance-data]
 api 9
 mov [second_uninit_result-data],ax
report:
 push ds
 push word result_name-data
 push word 0
 api 10
 cmp ax,-1
 je fatal
 mov [file-data],ax
 push ax
 push ds
 push word report_start-data
 push word report_end-report_start
 api 11
 cmp ax,report_end-report_start
 jne fatal
 push word [file-data]
 api 12
 mov ax,0x4c00
 int 0x21
fatal:
 mov ax,0x4c01
 int 0x21
; FNCALLBACK(UINT,UINT, HCONV,HSZ,HSZ,HDDEDATA,DWORD,DWORD):
; 4 + 4*4 + 8 = 28 argument bytes, far Pascal, DWORD return.
dde_callback:
 xor ax,ax
 xor dx,dx
 retf 28
code_end:
dw 12
%macro fixup 3
db 3,1
dw ..@call_%1-code+1,%2,%3
%endmacro
fixup 1,1,91
fixup 2,1,5
fixup 3,1,47
fixup 4,1,49
fixup 5,2,2
fixup 6,2,2
fixup 7,2,2
fixup 8,2,3
fixup 9,2,3
fixup 10,1,83
fixup 11,1,86
fixup 12,1,81
align 16,db 0
data:
times 32 db 0
report_start: db 'DDI1'
module_handle: dw 0
path_length: dw 0
invalid_result: dw 0xffff
init_result: dw 0xffff
init_id_lo: dw 0
init_id_hi: dw 0
reinit_result: dw 0xffff
reinit_id_lo: dw 0
reinit_id_hi: dw 0
uninit_result: dw 0xffff
second_uninit_result: dw 0xffff
module_path: times 260 db 0
report_end:
instance: dd 0
file: dw 0
module_name: db 'DDEML',0
result_name: db 'DINIT.BIN',0
data_end:
image_end:

