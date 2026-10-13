; Independently authored disposable NE test, never replacement guest media.
; Header shape follows the existing independent directory probe. Only original
; KERNEL InitTask(91)/WinExec(166) and USER InitApp(5) are imported; ordinals
; are checked against kernel31/kernel.def and user/user.def.
; A real Win16 parent starts immutable WINMINE, then exits. The outer probe
; waits for parent completion before checking the child's inherited policy.
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
dw (data-$$)/16,data_end-data,0x0c51,0x2000
resources: dw 0,0
resident: db 8,'STYLEPAR'
dw 0
db 0
modules: dw kernel-imports,user-imports
imports:
db 0
kernel: db 6,'KERNEL'
user: db 4,'USER'
entries: db 0
entries_end:
align 16,db 0
code:
init_call: db 0x9a
dw 0xffff,0
or ax,ax
jz failed
push di
init_app_call: db 0x9a
dw 0xffff,0
or ax,ax
jz failed
push ds
push word child-data
push word 1
exec_call: db 0x9a
dw 0xffff,0
cmp ax,32
jb failed
mov ax,0x4c00
int 0x21
failed:
mov ax,0x4c01
int 0x21
code_end:
dw 3
%macro fixup 3
db 3,1
dw %1-code+1,%2,%3
%endmacro
fixup init_call,1,91
fixup init_app_call,2,5
fixup exec_call,1,166
align 16,db 0
data:
times 32 db 0
%ifdef BASELINE_RETURN
child: db 'tests\STYLENAT.EXE wow-baseline-return',0
%elifdef NATIVE_RETURN
child: db 'tests\STYLENAT.EXE wow-return',0
%else
child: db 'WINMINE.EXE',0
%endif
data_end:
; Keep a complete first file sector for the host image classifier. The tiny
; unpadded fixture was classified as DOS before any WOW code was reached.
align 512,db 0
image_end:
