; Isolated Win16 diagnostic: call KERNEL GlobalDOSAlloc/GlobalDOSFree from
; an ordinary Windows application and record each stage. Never a guest patch.
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
resident: db 7,'M31ALOC'
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
align 16,db 0
code:
api 1,91
test ax,ax
jz fatal
push di
api 2,5
test ax,ax
jz fatal
push ds
push word filename-data
push word 0
api 1,83
cmp ax,-1
je fatal
mov [file-data],ax
mov word [phase-data],1
call checkpoint
%ifdef MOUSE31_VM_LAUNCH_CONTROL
; Control only: launch the same DOS child against video memory without
; allocating or retaining a GlobalDOS block.
mov word [segment_value-data],0xb800
%else
; DWORD size: high word first, as required by the Pascal convention.
push word 0
push word 32
api 1,184
mov [handle-data],ax
mov [segment_value-data],dx
%endif
mov word [phase-data],2
call checkpoint
%ifndef MOUSE31_VM_LAUNCH_CONTROL
cmp word [handle-data],0
je done
; Test the prototype's use of the returned handle as a selector separately
; from the allocation itself. A fault leaves the phase-2 record intact.
mov es,[handle-data]
mov word [es:0],0x5aa5
mov ax,[es:0]
mov [write_result-data],ax
%endif
mov word [phase-data],3
call checkpoint
%ifdef MOUSE31_VM_CHILD
; Give the DOS child the real-mode segment. The parent keeps the allocation
; alive until the child reports or the bounded diagnostic interval ends.
mov ax,[segment_value-data]
mov di,command_hex-data
mov cx,4
.hex:
rol ax,4
mov bx,ax
and bx,15
add bl,'0'
cmp bl,'9'
jbe .hex_store
add bl,7
.hex_store:
mov [di],bl
inc di
loop .hex
push ds
push word command-data
push word 1
api 1,166
mov [launch_result-data],ax
mov word [phase-data],4
call checkpoint
cmp word [launch_result-data],32
jb .child_done
api 2,13
mov [start_tick-data],ax
.wait_child:
api 1,29
push ds
push word child_file-data
push word 0
api 1,85
cmp ax,-1
jne .child_open
api 2,13
sub ax,[start_tick-data]
cmp ax,15000
jb .wait_child
jmp .child_done
.child_open:
push ax
api 1,81
mov word [child_seen-data],1
.child_done:
mov word [phase-data],5
call checkpoint
%endif
%ifndef MOUSE31_VM_LAUNCH_CONTROL
push word [handle-data]
api 1,185
mov [free_result-data],ax
%endif
%ifdef MOUSE31_VM_CHILD
mov word [phase-data],6
%else
mov word [phase-data],4
%endif
call checkpoint
done:
push word [file-data]
api 1,81
mov ax,0x4c00
int 0x21
checkpoint:
push word [file-data]
push ds
push word result-data
push word result_end-result
api 1,86
cmp ax,result_end-result
jne fatal
ret
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
result: db 'M31A'
phase: dw 0
handle: dw 0
segment_value: dw 0
free_result: dw 0xffff
write_result: dw 0
%ifdef MOUSE31_VM_CHILD
launch_result: dw 0
child_seen: dw 0
%endif
result_end:
file: dw 0
filename: db 'M31ALOC.BIN',0
%ifdef MOUSE31_VM_CHILD
start_tick: dw 0
command: db 'M31CHLD.COM '
command_hex: db '0000',0
child_file: db 'M31VM.BIN',0
%endif
data_end:
image_end:
