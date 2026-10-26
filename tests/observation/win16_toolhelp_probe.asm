; Independent NE witness for the immutable original WOW TOOLHELP.DLL.
; It exercises two retained guest query rows, their size-validation failure,
; and the original WOW ClassFirst UNIMPLEMENTEDAPI result.  It is not guest
; source and does not implement or replace any ToolHelp behavior.
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
resident: db 7,'THPROBE'
dw 0
db 0
modules: dw kernel-imports,toolhelp-imports
imports:
db 0
kernel: db 6,'KERNEL'
toolhelp: db 8,'TOOLHELP'
entries: db 0
entries_end:
%macro api 1
..@call_%1: db 0x9a
dw 0xffff,0
%endmacro
align 16,db 0
code:
api 1                         ; KERNEL!InitTask @91
test ax,ax
jz fatal
push di
api 2                         ; KERNEL!InitApp @5
test ax,ax
jz fatal
api 3                         ; KERNEL!GetVersion @3
mov [version-data],ax
api 4                         ; KERNEL!GetWinFlags @132
mov [winflags-data],ax

; TaskFirst requires exactly sizeof(TASKENTRY), not an approximate buffer.
mov dword [task-data],39
push ds
push word task-data
api 5                         ; TOOLHELP!TaskFirst @63
mov [task_ok-data],ax
test ax,ax
jz failed
cmp word [task+4-data],0
je failed

; ModuleFirst rejects the same buffer when its original size tag is absent.
mov dword [module-data],0
push ds
push word module-data
api 6                         ; TOOLHELP!ModuleFirst @59
mov [module_bad-data],ax
test ax,ax
jnz failed
mov dword [module-data],275  ; packed sizeof(MODULEENTRY)
push ds
push word module-data
api 7
mov [module_ok-data],ax
test ax,ax
jz failed

; The immutable WOW thunk table deliberately maps this export to
; UNIMPLEMENTEDAPI.  A nonzero result would be a semantic regression.
mov dword [class-data],264   ; packed sizeof(CLASSENTRY)
push ds
push word class-data
api 8                         ; TOOLHELP!ClassFirst @69
mov [class_result-data],ax
test ax,ax
jnz failed

mov word [exit_result-data],0
jmp write_report
failed:
mov word [exit_result-data],1
write_report:
push ds
push word result_name-data
push word 0
api 9                         ; KERNEL!_lcreat @83
cmp ax,-1
je fatal
mov [file-data],ax
push ax
push ds
push word report-data
push word report_end-report
api 10                        ; KERNEL!_lwrite @86
cmp ax,report_end-report
jne fatal
push word [file-data]
api 11                        ; KERNEL!_lclose @81
cmp word [exit_result-data],0
jne fatal
mov ax,0x4c00
int 0x21
fatal:
mov ax,0x4c01
int 0x21
code_end:
dw 11
%macro fixup 3
db 3,1
dw ..@call_%1-code+1,%2,%3
%endmacro
fixup 1,1,91
fixup 2,1,5
fixup 3,1,3
fixup 4,1,132
fixup 5,2,63
fixup 6,2,59
fixup 7,2,59
fixup 8,2,69
fixup 9,1,83
fixup 10,1,86
fixup 11,1,81
align 16,db 0
data:
times 32 db 0
report: db 'THP1'
version: dw 0
winflags: dw 0
task_ok: dw 0
module_bad: dw 0
module_ok: dw 0
class_result: dw 0
exit_result: dw 0
report_end:
file: dw 0
result_name: db 'RESULT.BIN',0
task: times 39 db 0
module: times 275 db 0
class: times 264 db 0
data_end:
image_end:
