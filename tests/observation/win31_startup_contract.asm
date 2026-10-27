; Independent Win16 integration witness; no host input or desktop automation.
; W31BOOT.BIN proves the Win16 probe started; W31READY.BIN records a live
; Program Manager HWND and GetWinFlags. ExitWindows requests guest shutdown.
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
resident: db 8,'W31CHECK'
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
api 1,91                         ; InitTask
test ax,ax
jz fatal
push di
api 2,5                          ; InitApp
test ax,ax
jz fatal
api 1,132                        ; GetWinFlags
mov [flags-data],ax
mov [flags+2-data],dx
push ds
push word boot_name-data
push word 0
api 1,83                         ; _lcreat
cmp ax,-1
je fatal
push ax
api 1,81                         ; _lclose
.wait:
api 1,29                         ; Yield: permit shell to create its window
push ds
push word class_name-data
push word 0                      ; NULL window title
push word 0
api 2,50                         ; FindWindow("Progman", NULL)
test ax,ax
jz .wait
mov [hwnd-data],ax
push ds
push word ready_name-data
push word 0
api 1,83                         ; _lcreat
cmp ax,-1
je fatal
mov [file-data],ax
push ax
push ds
push word report-data
push word report_end-report
api 1,86                         ; _lwrite
cmp ax,report_end-report
jne fatal
push word [file-data]
api 1,81                         ; _lclose
push word 0                      ; DWORD reserved
push word 0
push word 0                      ; UINT return code
api 2,7                          ; ExitWindows
mov ax,0x4c00
int 0x21
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
report: db 'W31C'
flags: dd 0
hwnd: dw 0
report_end:
file: dw 0
class_name: db 'Progman',0
; Runner replaces these fixed-width fields with paths in its owned copy.
boot_name: db '@W31_BOOT_PATH@',0
times 128-($-boot_name) db 0
ready_name: db '@W31_READY_PATH@',0
times 128-($-ready_name) db 0
data_end:
image_end:
