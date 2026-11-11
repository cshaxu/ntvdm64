; Independently authored Win16 multimedia test; never replacement guest media.
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
resident: db 7,'MMTPROB'
dw 0
db 0
modules: dw kernel-imports,user-imports

imports: db 0
kernel: db 6,'KERNEL'
user: db 4,'USER'
mmsystem: db 8,'MMSYSTEM'
helper: db 8,'MMTPROBE'
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
push di
api 2,5
test ax,ax
jz fatal
mov word [stop_result-data],1
call snapshot
push ds
push word mm_name-data
api 1,95
mov [timer-data],ax
push ax
push ds
push word module_path-data
push word 260
api 1,49
mov word [stop_result-data],2
call snapshot
push ds
push word dll_name-data
api 1,95
mov [before-data],ax
mov word [stop_result-data],3
call snapshot
%ifndef LOAD_ONLY
push word [before-data]
push word 0
push word 3
api 1,50
mov [start_ptr-data],ax
mov [start_ptr+2-data],dx
or ax,dx
jz fatal
push word [before-data]
push word 0
push word 4
api 1,50
mov [stop_ptr-data],ax
mov [stop_ptr+2-data],dx
or ax,dx
jz fatal
push word [before-data]
push word 0
push word 2
api 1,50
mov [query_ptr-data],ax
mov [query_ptr+2-data],dx
or ax,dx
jz fatal
push word [timer-data]
push word 0
push word 607
api 1,50
mov [time_ptr-data],ax
mov [time_ptr+2-data],dx
or ax,dx
jz fatal
%ifdef NO_TIMER
xor ax,ax
%else
call far [start_ptr-data]
%endif
mov [timer-data],ax
mov word [stop_result-data],10
call snapshot
call wait_interval
mov word [stop_result-data],11
call snapshot
call far [query_ptr-data]
mov [before-data],ax
mov [bad-data],dx
mov word [stop_result-data],12
call snapshot
%ifdef NO_TIMER
xor ax,ax
%else
call far [stop_ptr-data]
%endif
mov [stop_result-data],ax
call snapshot
call wait_interval
call far [query_ptr-data]
mov [settled-data],ax
call snapshot
call wait_interval
call far [query_ptr-data]
mov [final_count-data],ax
or [bad-data],dx
%endif
push ds
push word filename-data
push word 0
api 1,83
mov [file-data],ax
cmp ax,-1
je fatal
push ax
push ds
push word report-data
push word report_end-report
api 1,86
push word [file-data]
api 1,81
mov ax,0x4c00
int 0x21
fatal:
mov ax,0x4c01
int 0x21
snapshot:
push ax
push bx
push cx
push dx
mov dx,filename-data
xor cx,cx
mov ah,0x3c
int 0x21
jc .done
mov bx,ax
mov dx,report-data
mov cx,report_end-report
mov ah,0x40
int 0x21
mov ah,0x3e
int 0x21
.done:
pop dx
pop cx
pop bx
pop ax
ret
%ifndef LOAD_ONLY
wait_interval:
call far [time_ptr-data]
mov [tick-data],ax
.loop:
api 1,29
call far [time_ptr-data]
sub ax,[tick-data]
cmp ax,750
jb .loop
ret
%endif
code_end:
dw calls
%assign i 1
%rep calls
db 3,1
dw ..@call_%[i]-code+1,module_%[i],ordinal_%[i]
%assign i i+1
%endrep
align 16,db 0
data:
times 32 db 0
report: db 'MMT1'
timer: dw 0
before: dw 0
stop_result: dw 0xffff
settled: dw 0
final_count: dw 0
bad: dw 0
module_path: times 260 db 0
report_end:
start_ptr: dd 0
stop_ptr: dd 0
query_ptr: dd 0
time_ptr: dd 0
tick: dw 0
file: dw 0
filename: db 'MMTPROB.BIN',0
mm_name: db 'MMSYSTEM.DLL',0
dll_name: db 'MMTPROBE.DLL',0
data_end:
image_end:
