; Independent NE input: original KERNEL generic thunks -> native witness ->
; original WOWCallback16/Ex -> this program's PASCAL/CDECL callbacks.
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
dw 3,2,0
dw segments-ne,resources-ne,resident-ne,modules-ne,imports-ne
dd 0
dw 0,4,0
db 2,0
dw 0,0,0,0x030a
segments:
dw (code-$$)/16,code_end-code,0x0150,code_end-code
dw (data-$$)/16,data_end-data,0x0d51,0x4000
dw (demand-$$)/16,demand_end-demand,0x1030,demand_end-demand
resources: dw 0,0
resident: db 8,'GENPROBE'
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
%macro push32 1
push word [%1+2-data]
push word [%1-data]
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
%ifdef FAULT
api 19                        ; actual task identity before the failing thunk
mov [demand_selector-data],ax
%endif
call checkpoint
push cs
push word pascal_proc-code
push word [instance-data]
api 4
mov [pascal-data],ax
mov [pascal+2-data],dx
or ax,dx
jz fatal
push cs
push word cdecl_proc-code
push word [instance-data]
api 5
mov [cdecl-data],ax
mov [cdecl+2-data],dx
or ax,dx
jz fatal
push ds
push word library-data
push word 0
push word 0
push word 0
push word 0
api 6                         ; LoadLibraryEx32W
mov [module-data],ax
mov [module+2-data],dx
or ax,dx
jz fatal
push32 module
push ds
push word export-data
api 7                         ; GetProcAddress32W
mov [native-data],ax
mov [native+2-data],dx
or ax,dx
jz fatal
xor ax,ax
call invoke_native
push32 module
push ds
push word resolver_export-data
api 14
mov [resolver-data],ax
mov [resolver+2-data],dx
or ax,dx
jz fatal
call resolve_segment          ; establish resident bytes before discarding
; Use original GlobalDiscard contract on this probe's own unlocked code segment.
; ldutil.asm tags all data segments GA_DGROUP; code has the original discard path.
; 3gmem.asm grealloc(size0,MOVEABLE) calls gnotify and mark_sel_NP itself.
push word [demand_selector-data]
api 16                        ; GlobalHandle
test ax,ax
jz fatal
mov [demand_handle-data],ax
push ax
push word 0
push word 0
push word 2                   ; GMEM_MOVEABLE, not a descriptor modification
api 17                        ; GlobalReAlloc
test ax,ax
jz fatal
push word [demand_handle-data]
api 18                        ; GlobalFlags
test ax,0x4000                ; GMEM_DISCARDED must actually be true
jz fatal
call resolve_segment          ; resolve again after original mark_sel_NP
mov word [phase-data],1
call checkpoint
push32 pascal
api 8                         ; FreeProcInstance
inc word [freed-data]
push32 cdecl
api 9
inc word [freed-data]
push32 module
api 10                        ; FreeLibrary32W
test ax,ax
jz fatal
inc word [freed-data]
mov word [phase-data],2
call checkpoint
push word [file-data]
api 11
mov ax,0x4c00
int 0x21
fatal:
mov ax,0x4c01
int 0x21

resolve_segment:
mov [saved_sp-data],sp
push word [demand_selector-data]
push word 0
push32 resolver
push word 0
push word 0
push word 0
push word 1
api 15
cmp sp,[saved_sp-data]
jne fatal
cmp ax,1
jne fatal
cmp dx,0x447a
jne fatal
ret

; Variable Pascal arguments followed by target, conversion mask and count.
; Original kthunks.asm pops count*4+12. Keep a stack-local witness for nesting.
invoke_native:
push bp
mov bp,sp
push32 pascal
push32 cdecl
push word 0
push ax
push32 native
push word 0
push word 0
push word 0
push word 3
api 12                        ; CallProc32W
cmp sp,bp
jne fatal
cmp ax,1
jne .bad
cmp dx,0x447a
je .done
.bad: inc word [errors-data]
.done:
pop bp
ret
checkpoint:
push word [file-data]
push ds
push word report-data
push word report_end-report
api 13
cmp ax,report_end-report
jne fatal
ret
pascal_proc:
push bp
mov bp,sp
push ds
mov ds,ax
push bx
push si
push di
inc word [pascal_calls-data]
cmp word [bp+6],0x3040
jne .return
cmp word [bp+8],0x1020
jne .return
inc word [nested_calls-data]
mov ax,1
call invoke_native
.return:
mov ax,[bp+6]
mov dx,[bp+8]
add ax,1
adc dx,0
pop di
pop si
pop bx
pop ds
pop bp
retf 4
cdecl_proc:
push bp
mov bp,sp
push ds
mov ds,ax
inc word [cdecl_calls-data]
mov ax,[bp+6]
mov dx,[bp+8]
add ax,1
adc dx,0
pop ds
pop bp
retf
code_end:
%ifdef FAULT
dw 19
%else
dw 18
%endif
%macro fixup 3
db 3,1
dw ..@call_%1-code+1,%2,%3
%endmacro
fixup 1,1,91
fixup 2,2,5
fixup 3,1,83
fixup 4,1,51
fixup 5,1,51
fixup 6,1,513
fixup 7,1,515
fixup 8,1,52
fixup 9,1,52
fixup 10,1,514
fixup 11,1,81
fixup 12,1,517
fixup 13,1,86
fixup 14,1,515
fixup 15,1,517
fixup 16,1,21
fixup 17,1,16
fixup 18,1,22
%ifdef FAULT
fixup 19,1,36
%endif
align 16,db 0
data:
times 32 db 0
report: db 'GEN1'
phase: dw 0
errors: dw 0
pascal_calls: dw 0
cdecl_calls: dw 0
nested_calls: dw 0
freed: dw 0
demand_selector: dw 0xffff
report_end:
instance: dw 0
file: dw 0
pascal: dd 0
cdecl: dd 0
module: dd 0
native: dd 0
resolver: dd 0
saved_sp: dw 0
demand_handle: dw 0
library: db 'GENHOST.DLL',0
export: db 'RunGenericProbe',0
resolver_export: db 'ResolveGenericSegment',0
filename: db 'GENPROBE.BIN',0
data_end:
dw 1
db 2,0                         ; internal selector-only relocation to segment3
dw demand_selector-data,3,0
align 16,db 0
demand: dd 0x447a1234
demand_end:
image_end:
