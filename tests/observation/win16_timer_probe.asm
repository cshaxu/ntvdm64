; Independent NE witness, not replacement guest media. USER ordinals are
; retained in src/mvdm/wow16/user/user.def; KERNEL ABI follows the CBT probe.
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
resident: db 8,'TMRPROBE'
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
api 1                         ; InitTask
test ax,ax
jz fatal
mov [instance-data],di
push di
api 2                         ; InitApp
test ax,ax
jz fatal
push ds
push word filename-data
push word 0
api 3                         ; _lcreat
cmp ax,-1
je fatal
mov [file-data],ax
call checkpoint
push cs
push word timer_proc-code
push word [instance-data]
api 4                         ; MakeProcInstance
mov [proc-data],ax
mov [proc-data+2],dx
or ax,dx
jz fatal
push cs
push word font_proc-code
push word [instance-data]
api 15                        ; MakeProcInstance for synchronous GDI callback
mov [fontproc-data],ax
mov [fontproc-data+2],dx
or ax,dx
jz fatal
push word 0
api 16                        ; GetDC(NULL), process-owned test lease
test ax,ax
jz fatal
mov [dc-data],ax
mov word [phase-data],1
call start_timer
.pump:
push ds
push word message-data
push word 0
push word 0
push word 0
api 5                         ; GetMessage
test ax,ax
jz fatal
push ds
push word message-data
api 6                         ; DispatchMessage
cmp word [ordinary-data],1
jb .pump
call checkpoint
mov word [phase-data],2
call start_timer
push word 0
push ds
push word text-data
push ds
push word title-data
push word 0
api 7                         ; MessageBox: native modal dispatch, no peer clicks
mov [modal_result-data],ax
cmp ax,1
jne bad_result
cmp word [modal-data],1
je cleanup
bad_result:
inc word [errors-data]
cleanup:
push word 0
push word [dc-data]
api 17                        ; ReleaseDC
test ax,ax
jnz .released
inc word [errors-data]
.released:
push word [fontproc-data+2]
push word [fontproc-data]
api 18                        ; FreeProcInstance (font)
push word [proc-data+2]
push word [proc-data]
api 8                         ; FreeProcInstance
mov word [freed-data],1
call checkpoint
push word [file-data]
api 9                         ; _lclose
cmp word [errors-data],0
jne fatal
mov ax,0x4c00
int 0x21
fatal:
mov ax,0x4c01
int 0x21

start_timer:
push word 0
push word 0
push word 200
push word [proc-data+2]
push word [proc-data]
api 10                        ; SetTimer(NULL,0,200,proc)
test ax,ax
jz fatal
mov [timer-data],ax
ret
checkpoint:
push word [file-data]
push ds
push word report-data
push word report_end-report
api 11                        ; _lwrite
cmp ax,report_end-report
jne fatal
ret

; FAR PASCAL TimerProc(HWND,UINT,UINT,DWORD), DS supplied by MakeProcInstance.
timer_proc:
push bp
mov bp,sp
push ds
mov ds,ax
push bx
push si
push di
push es
cmp word [bp+12],0x113         ; WM_TIMER
jne .bad
mov ax,[bp+10]
cmp ax,[timer-data]
jne .bad
push word 0
push ax
api 12                        ; KillTimer, exact original returned identity
test ax,ax
jz .bad
inc word [killed-data]
mov [saved_sp-data],sp
push word [dc-data]
push word 0
push word 0                  ; enumerate any face, stop on first callback
push word [fontproc-data+2]
push word [fontproc-data]
push word 0x1234
push word 0x447a
api 19                        ; EnumFonts, nested non-WND guest callback
cmp sp,[saved_sp-data]
jne .bad
test ax,ax                   ; callback returned zero: stop
jnz .bad
cmp word [phase-data],1
jne .modal
inc word [ordinary-data]
jmp .return
.modal:
cmp word [phase-data],2
jne .bad
inc word [modal-data]
api 13                        ; GetActiveWindow (native message-box HWND)
test ax,ax
jz .bad
push ax
push word 1
api 14                        ; EndDialog(IDOK), return through original modal loop
; Win16 EndDialog is void (windows.h / WU32EndDialog), unlike Win32 BOOL.
; The outer MessageBox result and completed modal phase verify its effect.
jmp .return
.bad:
inc word [errors-data]
.return:
pop es
pop di
pop si
pop bx
pop ds
pop bp
retf 10

; FAR PASCAL FontEnumProc(LOGFONT far*,TEXTMETRIC far*,WORD,DWORD).
; The real original W32EnumFontFunc writes both guest buffers before entry.
font_proc:
push bp
mov bp,sp
push ds
mov ds,ax
push bx
push si
push di
push es
inc word [fonts-data]
cmp word [bp+6],0x447a
jne .bad
cmp word [bp+8],0x1234
jne .bad
cmp word [bp+18],0
je .bad
cmp word [bp+14],0
je .bad
les di,[bp+16]
mov dx,[es:di]               ; consume original converted LOGFONT16
les di,[bp+12]
xor dx,[es:di]               ; consume original converted TEXTMETRIC16
xor [font_checksum-data],dx
inc word [font_reads-data]
jmp .return
.bad:
inc word [errors-data]
.return:
pop es
pop di
pop si
pop bx
pop ds
pop bp
xor ax,ax
xor dx,dx
retf 14
code_end:
dw 19
%macro fixup 3
db 3,1
dw ..@call_%1-code+1,%2,%3
%endmacro
fixup 1,1,91
fixup 2,2,5
fixup 3,1,83
fixup 4,1,51
fixup 5,2,108
fixup 6,2,114
fixup 7,2,1
fixup 8,1,52
fixup 9,1,81
fixup 10,2,10
fixup 11,1,86
fixup 12,2,12
fixup 13,2,60
fixup 14,2,88
fixup 15,1,51
fixup 16,2,66
fixup 17,2,68
fixup 18,1,52
fixup 19,3,70
align 16,db 0
data:
times 32 db 0
report: db 'TMR1'
phase: dw 0
errors: dw 0
ordinary: dw 0
modal: dw 0
killed: dw 0
modal_result: dw 0
freed: dw 0
fonts: dw 0
font_reads: dw 0
report_end:
instance: dw 0
file: dw 0
proc: dd 0
timer: dw 0
fontproc: dd 0
dc: dw 0
saved_sp: dw 0
font_checksum: dw 0
message: times 18 db 0
text: db 'The Win16 timer closes this native modal loop.',0
title: db 'S2 timer callback witness',0
filename: db 'TMRPROBE.BIN',0
data_end:
image_end:
