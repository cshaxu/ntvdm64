; Independently authored NE test input, never replacement guest media.
; Exercise original INT21 vector registration -> taskman -> INT2F:1689.
bits 16
%ifndef TICK_LIMIT
%define TICK_LIMIT 2
%endif
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
; These are distinct modules in SharedTasks, not two instances of one image.
; Original LMAlreadyLoaded also searches resident module names.
resident:
%ifdef PEER
db 7,'IDLPEER'
%else
db 8,'IDLPROBE'
%endif
dw 0
db 0
modules: dw kernel-imports,user-imports
imports:
db 0
kernel: db 6,'KERNEL'
user: db 4,'USER'
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
push ds
push word filename-data
push word 0
api 3
cmp ax,-1
je fatal
mov [file-data],ax
api 4
mov [task-data],ax
call checkpoint
%ifndef NO_REGISTER
mov ax,0x352f
int 0x21
mov [oldvector-data],bx
mov [oldvector-data+2],es
push ds
push cs
pop ds
mov dx,idle_interrupt-code
mov ax,0x252f
int 0x21
pop ds
%endif
mov word [phase-data],1
push word 0
push word 0
push word 400
push word 0
push word 0
api 5                         ; SetTimer: bound the otherwise idle wait
mov [timer-data],ax
test ax,ax
jz restore_failed
.pump:
push ds
push word message-data
push word 0
push word 0
push word 0
api 6                         ; GetMessage enters original taskman wait
test ax,ax
jz restore_failed
%ifdef SHARED_WITNESS
mov ax,[message-data+2]
mov [last_message-data],ax
call checkpoint               ; retain progress even on a failed deadline
%endif
cmp word [message-data+2],0x113
jne .pump
inc word [ticks-data]
cmp word [ticks-data],TICK_LIMIT
jb .pump
%ifndef NO_REGISTER
cmp word [idles-data],0
jne .observed
inc word [errors-data]
.observed:
%endif
push word 0
push word [timer-data]
api 7
test ax,ax
jnz restore
restore_failed:
inc word [errors-data]
restore:
call checkpoint
%ifndef NO_REGISTER
push ds
mov dx,[oldvector-data]
mov ax,[oldvector-data+2]
mov ds,ax
mov ax,0x252f
int 0x21
pop ds
mov ax,0x352f
int 0x21
cmp bx,[oldvector-data]
jne .bad_restore
mov ax,es
cmp ax,[oldvector-data+2]
jne .bad_restore
mov word [restored-data],1
jmp .finish
.bad_restore:
inc word [errors-data]
%else
mov word [restored-data],2       ; explicitly never registered a vector
%endif
.finish:
mov word [phase-data],2
call checkpoint
push word [file-data]
api 8
cmp word [errors-data],0
jne fatal
mov ax,0x4c00
int 0x21
fatal:
mov ax,0x4c01
int 0x21
checkpoint:
push word [file-data]
push ds
push word report-data
push word report_end-report
api 9
cmp ax,report_end-report
jne fatal
ret

idle_interrupt:
push ax
push ds
push bp
mov bp,sp
data_selector: mov ax,0xffff   ; ordinary NE internal selector relocation
mov ds,ax
cmp word [ss:bp+4],0x1689
jne .chain
inc word [idles-data]
%ifdef SHARED_WITNESS
push bx
push cx
push dx
push es
api 10                         ; task executing A's vector, not vector owner
cmp ax,[task-data]
je .own_task
inc word [cross_count-data]
mov [cross_task-data],ax
.own_task:
pop es
pop dx
pop cx
pop bx
%endif
pop bp
pop ds
pop ax
iret
.chain:
push word [oldvector-data+2]
push word [oldvector-data]
mov ax,[ss:bp+4]
mov ds,[ss:bp+2]
mov bp,[ss:bp]
retf 6                        ; retain original interrupt frame and registers
code_end:
%ifdef SHARED_WITNESS
dw 11
%else
dw 10
%endif
%macro fixup 3
db 3,1
dw ..@call_%1-code+1,%2,%3
%endmacro
fixup 1,1,91
fixup 2,2,5
fixup 3,1,83
fixup 4,1,36
fixup 5,2,10
fixup 6,2,108
fixup 7,2,12
fixup 8,1,81
fixup 9,1,86
%ifdef SHARED_WITNESS
fixup 10,1,36
%endif
db 2,0
dw data_selector-code+1
db 2,0
dw 0
align 16,db 0
data:
times 32 db 0
report: db 'IDL1'
phase: dw 0
errors: dw 0
idles: dw 0
ticks: dw 0
restored: dw 0
task: dw 0
%ifdef SHARED_WITNESS
cross_count: dw 0
cross_task: dw 0
last_message: dw 0
%endif
report_end:
file: dw 0
timer: dw 0
oldvector: dd 0
message: times 18 db 0
%ifdef PEER
filename: db 'IDLPEER.BIN',0
%else
filename: db 'IDLPROBE.BIN',0
%endif
data_end:
image_end:
