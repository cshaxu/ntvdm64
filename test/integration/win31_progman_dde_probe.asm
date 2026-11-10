; Win16 DDE client for the original guest Program Manager; test only.
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
resident: db 8,'W31CHECK'
dw 0
db 0
modules: dw kernel-imports,user-imports,ddeml-imports
imports:
db 0
kernel: db 6,'KERNEL'
user: db 4,'USER'
ddeml: db 5,'DDEML'
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

; Test-only DDE client of the real Win3.1 Program Manager.
mov word [dde_stage-data],1
push ds
push word dde_id-data
push cs
push word dde_callback-code
push word 0
push word 0x10
push word 0
push word 0
api 3,2
mov [dde_result-data],ax
or ax,ax
jnz .dde_finish
mov word [dde_stage-data],2
push word [dde_id+2-data]
push word [dde_id-data]
push ds
push word dde_service-data
push word 0
api 3,21
mov [dde_service_h-data],ax
mov [dde_service_h+2-data],dx
or ax,dx
jz .dde_finish
mov word [dde_stage-data],3
push word [dde_id+2-data]
push word [dde_id-data]
push ds
push word dde_service-data
push word 0
api 3,21
mov [dde_topic_h-data],ax
mov [dde_topic_h+2-data],dx
or ax,dx
jz .dde_finish
mov word [dde_stage-data],4
push word [dde_id+2-data]
push word [dde_id-data]
push word [dde_service_h+2-data]
push word [dde_service_h-data]
push word [dde_topic_h+2-data]
push word [dde_topic_h-data]
push word 0
push word 0
api 3,7
mov [dde_conv-data],ax
mov [dde_conv+2-data],dx
or ax,dx
jz .dde_finish
mov word [dde_stage-data],5
%macro dde_execute 2
push ds
push word %1-data
push word 0
push word %2-%1
push word [dde_conv+2-data]
push word [dde_conv-data]
push word 0
push word 0
push word 0
push word 0x4050
push word 0
push word 5000
push word 0
push word 0
api 3,11
mov [dde_transaction-data],ax
mov [dde_transaction+2-data],dx
or ax,dx
jz .dde_finish
inc word [dde_result-data]
%endmacro
dde_execute dde_main,dde_main_end
dde_execute dde_fileman,dde_fileman_end
dde_execute dde_accessories,dde_accessories_end
dde_execute dde_notepad,dde_notepad_end
dde_execute dde_games,dde_games_end
dde_execute dde_winmine,dde_winmine_end
dde_execute dde_startup,dde_startup_end
mov word [dde_stage-data],6
push word [dde_conv+2-data]
push word [dde_conv-data]
api 3,8
push word [dde_id+2-data]
push word [dde_id-data]
push word [dde_service_h+2-data]
push word [dde_service_h-data]
api 3,22
push word [dde_id+2-data]
push word [dde_id-data]
push word [dde_topic_h+2-data]
push word [dde_topic_h-data]
api 3,22
push word [dde_id+2-data]
push word [dde_id-data]
api 3,3
.dde_finish:
push ds
push word dde_result_name-data
push word 0
api 1,83
cmp ax,-1
je fatal
mov [file-data],ax
push ax
push ds
push word dde_stage-data
push word 8
api 1,86
push word [file-data]
api 1,81
push word 0                      ; DWORD reserved
push word 0
push word 0                      ; UINT return code
api 2,7                          ; ExitWindows
mov ax,0x4c00
int 0x21
fatal:
mov ax,0x4c01
int 0x21
dde_callback:
xor ax,ax
xor dx,dx
retf 28
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
dde_stage: dw 0
dde_result: dw 0
dde_transaction: dd 0
dde_id: dd 0
dde_service_h: dd 0
dde_topic_h: dd 0
dde_conv: dd 0
dde_service: db 'PROGMAN',0
dde_main: db '[CreateGroup(Main,O:\tmp\q31\PATCH\DDEMAIN.GRP)]',0
dde_main_end:
dde_fileman: db '[AddItem(O:\tmp\q31\WINFILE.EXE,File Manager)]',0
dde_fileman_end:
dde_accessories: db '[CreateGroup(Accessories,O:\tmp\q31\PATCH\DDEACC.GRP)]',0
dde_accessories_end:
dde_notepad: db '[AddItem(O:\tmp\q31\NOTEPAD.EXE,Notepad)]',0
dde_notepad_end:
dde_games: db '[CreateGroup(Games,O:\tmp\q31\PATCH\DDEGAME.GRP)]',0
dde_games_end:
dde_winmine: db '[AddItem(O:\tmp\q31\WINMINE.EXE,Minesweeper)]',0
dde_winmine_end:
dde_startup: db '[CreateGroup(StartUp,O:\tmp\q31\PATCH\DDESTART.GRP)]',0
dde_startup_end:
dde_result_name: db 'O:\tmp\q31\PATCH\DDE.BIN',0
data_end:
image_end:
