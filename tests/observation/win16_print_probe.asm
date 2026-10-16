; Independently authored NE test input, not a replacement guest component.
; Explicit Microsoft Print to PDF output; never use the default printer.
bits 16
org 0
%ifndef OUTPUT_DIR
%error OUTPUT_DIR must name the owned test output directory
%endif
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
resident: db 8,'PRNPROBE'
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
%macro api 1
..@call_%1: db 0x9a
dw 0xffff,0
%endmacro
align 16,db 0
code:
api 1
test ax,ax
jz fatal
mov [instance-data],di
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
push cs
push word abort_proc-code
push word [instance-data]
api 5
mov [proc-data],ax
mov [proc-data+2],dx
or ax,dx
jz fatal
mov [docinfo-data+4],ds
mov [docinfo-data+8],ds
call checkpoint
next_document:
push ds
push word driver-data
push ds
push word printer-data
push word 0
push word 0
push word 0
push word 0
api 6                         ; CreateDC
test ax,ax
jz fatal
mov [dc-data],ax
push ax
push word [proc-data+2]
push word [proc-data]
api 7                         ; SetAbortProc
cmp ax,0
jle fatal
mov ax,normal_output-data
cmp word [phase-data],0
je .output
mov ax,cancel_output-data
.output:
mov [docinfo-data+6],ax
push word [dc-data]
push ds
push word docinfo-data
api 8                         ; StartDoc (marshals DOCINFO16 strings)
cmp ax,0
jle fatal
push word [dc-data]
api 9                         ; StartPage
cmp ax,0
jle fatal
mov word [drawing-data],1
; Original GDI checks the abort callback on a roughly250ms interval.
; Permit that interval to elapse, but also bound a stalled guest clock.
mov word [remaining-data],0xffff
api 10                        ; USER.GetTickCount
mov [started-data],ax
.draw:
push word [dc-data]
push word 10
push word 10
push word 100
push word 100
api 11                        ; Rectangle
api 12                        ; Yield without another task scheduler
api 13                        ; GetTickCount
sub ax,[started-data]
cmp ax,1000
jae .end_page
dec word [remaining-data]
jnz .draw
inc word [errors-data]
.end_page:
push word [dc-data]
api 14                        ; EndPage
mov word [drawing-data],0
cmp word [phase-data],0
jne .cancelled
cmp ax,0
jg .end_document
inc word [errors-data]
jmp .end_document
.cancelled:
cmp ax,0
jle .end_document
inc word [errors-data]
.end_document:
push word [dc-data]
api 15                        ; EndDoc succeeds also if already aborted
cmp ax,0
jg .delete
inc word [errors-data]
.delete:
push word [dc-data]
api 16
test ax,ax
jnz .deleted
inc word [errors-data]
.deleted:
inc word [deleted-data]
inc word [phase-data]
call checkpoint
cmp word [phase-data],2
jb next_document
push word [proc-data+2]
push word [proc-data]
api 17
mov word [freed-data],1
call checkpoint
push word [file-data]
api 18
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
api 19
cmp ax,report_end-report
jne fatal
ret
; FAR PASCAL BOOL AbortProc(HDC,int); MakeProcInstance supplies DS in AX.
abort_proc:
push bp
mov bp,sp
push ds
mov ds,ax
push bx
push si
push di
push es
mov ax,[bp+8]
cmp ax,[dc-data]
jne .bad
api 20
cmp ax,[task-data]
je .count
.bad:
inc word [errors-data]
.count:
cmp word [phase-data],0
jne .cancel
inc word [normal_calls-data]
jmp .yes
.cancel:
inc word [cancel_calls-data]
cmp word [drawing-data],0
je .yes
inc word [cancelled-data]
xor ax,ax
jmp .return
.yes:
mov ax,1
.return:
pop es
pop di
pop si
pop bx
pop ds
pop bp
retf 4
code_end:
dw 20
%macro fixup 3
db 3,1
dw ..@call_%1-code+1,%2,%3
%endmacro
fixup 1,1,91
fixup 2,2,5
fixup 3,1,83
fixup 4,1,36
fixup 5,1,51
fixup 6,3,53
fixup 7,3,381
fixup 8,3,377
fixup 9,3,379
fixup 10,2,13
fixup 11,3,27
fixup 12,1,29
fixup 13,2,13
fixup 14,3,380
fixup 15,3,378
fixup 16,3,68
fixup 17,1,52
fixup 18,1,81
fixup 19,1,86
fixup 20,1,36
align 16,db 0
data:
times 32 db 0
report: db 'PRN1'
phase: dw 0
errors: dw 0
normal_calls: dw 0
cancel_calls: dw 0
cancelled: dw 0
deleted: dw 0
freed: dw 0
task: dw 0
report_end:
instance: dw 0
file: dw 0
proc: dd 0
dc: dw 0
remaining: dw 0
started: dw 0
drawing: dw 0
docinfo: dw 10,docname-data,0,normal_output-data,0
docname: db 'T447 independent Win16 callback witness',0
driver: db 'WINSPOOL',0
printer: db 'Microsoft Print to PDF',0
filename: db 'PRNPROBE.BIN',0
normal_output: db OUTPUT_DIR,'\normal.pdf',0
cancel_output: db OUTPUT_DIR,'\cancel.pdf',0
data_end:
image_end:
