; Independently authored NE witness for the original raw OPENJOB failure edge.
; It must not configure a printer or replace the legacy [devices] profile rule.
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
resident: db 8,'RAWPROBE'
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
api 1                         ; KERNEL.InitTask
test ax,ax
jz fatal
mov [instance-data],di
push di
api 2                         ; KERNEL.InitApp
test ax,ax
jz fatal
push ds
push word filename-data
push word 0
api 3                         ; KERNEL._lcreat
cmp ax,-1
je fatal
mov [file-data],ax
; OpenJob(LPSTR output, LPSTR docName, HPJOB previousJob), 10 bytes.
; The original GDI thunk and wspool.c must perform the profile lookup.
push ds
push word output-data
push ds
push word document-data
push word 0
api 4                         ; GDI.OpenJob @240
mov [openjob-data],ax
test ax,ax
jz report
push ax
api 5                         ; GDI.CloseJob @243; unexpected success still retires it.
mov [closejob-data],ax
report:
push word [file-data]
push ds
push word record-data
push word record_end-record
api 6                         ; KERNEL._lwrite
cmp ax,record_end-record
jne fatal
push word [file-data]
api 7                         ; KERNEL._lclose
mov ax,0x4c00
int 0x21
fatal:
mov ax,0x4c01
int 0x21
code_end:
dw 7
%macro fixup 3
db 3,1
dw ..@call_%1-code+1,%2,%3
%endmacro
fixup 1,1,91
fixup 2,1,5
fixup 3,1,83
fixup 4,3,240
fixup 5,3,243
fixup 6,1,86
fixup 7,1,81
align 16,db 0
data:
times 32 db 0
record: db 'RAW1'
openjob: dw 0
closejob: dw 0
record_end:
instance: dw 0
file: dw 0
filename: db 'RAWPROBE.BIN',0
output: db 'FILE:',0
document: db 'T468 raw-spool profile witness',0
data_end:
image_end:
