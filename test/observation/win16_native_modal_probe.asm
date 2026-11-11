; Independent NE input, not a modified/rebuilt original guest.
; Original KERNEL/USER/SHELL ordinal contracts and WU32MessageBox/WS32ShellAbout.
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
resources: dw 0,0
resident: db 8,'MODAL447'
dw 0
db 0
modules: dw kernel-imports,user-imports,shell-imports
imports:
db 0
kernel: db 6,'KERNEL'
user: db 4,'USER'
shell: db 5,'SHELL'
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
api 1,91                     ; InitTask; DI becomes the task instance.
test ax,ax
jz fatal
push di
api 2,5                      ; InitApp
test ax,ax
jz fatal
api 1,36                     ; GetCurrentTask
mov [task-data],ax
push ds
push word filename-data
push word 0
api 1,83                     ; _lcreat
cmp ax,-1
je fatal
mov [file-data],ax
call checkpoint
push word 0
push ds
push word text-data
push ds
push word caption-data
push word 0                  ; MB_OK
api 2,1                      ; Actual USER16 MessageBox -> original thunk.
mov [message_result-data],ax
cmp ax,1
jne failed
mov word [phase-data],1
call checkpoint
push word 0
push ds
push word application-data
push ds
push word text-data
push word 0
api 3,22                     ; Actual SHELL16 ShellAbout -> original thunk.
mov [about_result-data],ax
cmp ax,1
jne failed
mov word [phase-data],2
call checkpoint
push word [file-data]
api 1,81
mov ax,0x4c00
int 0x21
failed:
mov word [errors-data],1
call checkpoint
fatal:
mov ax,0x4c01
int 0x21
checkpoint:
push word [file-data]
push ds
push word report-data
push word report_end-report
api 1,86
cmp ax,report_end-report
jne fatal
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
report: db 'MDL1'
task: dw 0
phase: dw 0
message_result: dw 0
about_result: dw 0
errors: dw 0
report_end:
file: dw -1
filename: db 'MODAL.BIN',0
caption: db 'T447 MessageBox scope',0
application: db 'T447 Modal scope',0
text: db 'Original guest modal return witness',0
data_end:
align 16,db 0
image_end:
