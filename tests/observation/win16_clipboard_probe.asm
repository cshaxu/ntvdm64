; Independently authored Win16 Clipboard witness.  It is assembled into a
; disposable test input and never replaces or rebuilds any guest image.
; The two directions deliberately use the selected original USER/KERNEL
; entries: guest CF_TEXT -> native verifier, then native CF_TEXT -> guest.
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
resident: db 7,'CLIPPRB'
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
%macro push32 1
push word [%1+2-data]
push word [%1-data]
%endmacro
align 16,db 0
code:
api 1,91                         ; KERNEL InitTask
test ax,ax
jz fatal
mov [instance-data],di
; Establish the report as soon as the original task exists.  A later missing
; report must therefore mean InitTask itself did not return, not merely that
; USER initialization or the Clipboard witness failed.
push ds
push word filename-data
push word 0
api 1,83                         ; KERNEL _lcreat
cmp ax,-1
je fatal
mov [file-data],ax
mov word [phase-data],12         ; original KERNEL task initialized
call checkpoint
push di
api 2,5                          ; USER InitApp
test ax,ax
jz fatal
mov word [phase-data],8          ; report file and USER task initialized
call checkpoint
; The callback witness must use the same original per-instance WNDPROC
; construction as real Win16 applications.  A stock BUTTON window would
; never exercise WOW's W32Win16WndProcEx route for clipboard messages.
push cs
push word window_proc-code
push word [instance-data]
api 1,51                         ; KERNEL MakeProcInstance
mov [procedure-data],ax
mov [procedure-data+2],dx
or ax,dx
jz fatal
mov word [phase-data],9          ; original MakeProcInstance completed
call checkpoint
mov ax,[procedure-data]
mov [wndclass+2-data],ax
mov ax,[procedure-data+2]
mov [wndclass+4-data],ax
mov ax,[instance-data]
mov [wndclass+10-data],ax
mov ax,ds
mov [wndclass+24-data],ax
push ds
push word wndclass-data
api 2,57                         ; USER RegisterClass
test ax,ax
jz fatal
mov word [phase-data],10         ; original RegisterClass completed
call checkpoint
; Use an actual task-owned, hidden Win16 window as the Clipboard owner.  NULL
; ownership is legal but does not exercise the normal owner lifetime that
; original applications use across a CloseClipboard handoff.
push ds
push word classname-data
push ds
push word title-data
push word 0x8000                 ; WS_POPUP, private-desktop hidden owner
push word 0
push word 0
push word 0
push word 80
push word 24
push word 0
push word 0
push word [instance-data]
push word 0
push word 0
api 2,41                          ; USER CreateWindow
test ax,ax
jz fatal
mov [owner-data],ax
mov word [phase-data],11         ; original CreateWindow completed
call checkpoint
mov word [phase-data],1
call checkpoint
%ifdef OWNER_TASK_EXIT
jmp owner_task_exit
%endif

; Resolve the native peer before this task becomes the Clipboard owner.  The
; exchange below still uses only original USER clipboard conversions.
push ds
push word library-data
push word 0
push word 0
push word 0
push word 0
api 1,513                        ; KERNEL LoadLibraryEx32W
mov [module-data],ax
mov [module-data+2],dx
mov [native_handle-data],ax
or ax,dx
jz failed
push32 module
push ds
push word export-data
api 1,515                        ; KERNEL GetProcAddress32W
mov [native-data],ax
mov [native-data+2],dx
mov [native_handle-data],ax
or ax,dx
jz failed

%ifdef CALLBACK_RETURN_ONLY
jmp callback_return_only
%endif
; Original USER open/empty/set accepts a guest-owned DDESHARE HMEM16. After
; SetClipboardData succeeds, the native verifier proves the host copy is the
; intended data; the guest handle is never passed as a raw native HGLOBAL.
push word [owner-data]
api 2,137                        ; USER OpenClipboard
test ax,ax
jz failed
api 2,139                        ; USER EmptyClipboard
test ax,ax
jz failed
push word 0x2002                 ; GMEM_MOVEABLE | GMEM_DDESHARE
push word 0
push word guest_payload_end-guest_payload
api 1,15                         ; KERNEL GlobalAlloc
test ax,ax
jz failed
mov [guest_handle-data],ax
push ax
api 1,18                         ; KERNEL GlobalLock -> DX:AX
mov di,ax
mov es,dx
or ax,dx
jz failed
mov si,guest_payload-data
mov cx,guest_payload_end-guest_payload
cld
rep movsb
push word [guest_handle-data]
api 1,19                         ; KERNEL GlobalUnlock
push word 1                      ; CF_TEXT
push word [guest_handle-data]
api 2,141                        ; USER SetClipboardData
cmp ax,[guest_handle-data]
jne failed
api 2,138                        ; USER CloseClipboard
test ax,ax
jz failed
mov word [phase-data],2
call checkpoint

mov ax,1
call invoke_bridge               ; native reads exact guest text
mov word [phase-data],3
call checkpoint
mov ax,2
call invoke_bridge               ; native publishes exact native text

; The original WU32GetClipboardData conversion must now return a guest HMEM16
; whose bytes equal the native text.  This is deliberately not a host-side
; pointer comparison.
push word [owner-data]
api 2,137                        ; USER OpenClipboard
test ax,ax
jz failed
push word 1                      ; CF_TEXT
api 2,142                        ; USER GetClipboardData
test ax,ax
jz failed
mov [native_handle-data],ax
push ax
api 1,18                         ; KERNEL GlobalLock
mov di,ax
mov es,dx
or ax,dx
jz failed
mov si,native_payload-data
mov cx,native_payload_end-native_payload
cld
repe cmpsb
jne failed
push word [native_handle-data]
api 1,19                         ; KERNEL GlobalUnlock
api 2,138                        ; USER CloseClipboard
test ax,ax
jz failed
mov word [phase-data],4
call checkpoint

; A rejected SetClipboardData must neither report success nor publish the
; guest input through the original cache. The native peer proves that the
; preceding native payload still owns the public Clipboard.
push word 0x2002
push word 0
push word rejected_payload_end-rejected_payload
api 1,15                         ; KERNEL GlobalAlloc
test ax,ax
jz failed
mov [rejected_handle-data],ax
push ax
api 1,18                         ; KERNEL GlobalLock -> DX:AX
mov di,ax
mov es,dx
or ax,dx
jz failed
mov si,rejected_payload-data
mov cx,rejected_payload_end-rejected_payload
cld
rep movsb
push word [rejected_handle-data]
api 1,19                         ; KERNEL GlobalUnlock
push word 1                      ; CF_TEXT, deliberately without OpenClipboard
push word [rejected_handle-data]
api 2,141                        ; USER SetClipboardData
mov [rejected_result-data],ax
mov ax,3
call invoke_bridge               ; host Clipboard must still hold native text
push word [owner-data]
api 2,137                        ; USER OpenClipboard
test ax,ax
jz failed
push word 1
api 2,142                        ; USER GetClipboardData
mov [rejected_observed-data],ax
test ax,ax
jz rejection_bad
push ax
api 1,18                         ; KERNEL GlobalLock
mov di,ax
mov es,dx
or ax,dx
jz rejection_bad
mov si,native_payload-data
mov cx,native_payload_end-native_payload
cld
repe cmpsb
jne rejection_bad
push word [rejected_observed-data]
api 1,19                         ; KERNEL GlobalUnlock
jmp rejection_done
rejection_bad:
inc word [errors-data]
rejection_done:
api 2,138                        ; USER CloseClipboard
test ax,ax
jz failed
push word [rejected_handle-data] ; caller retains failed input ownership
api 1,17                         ; KERNEL GlobalFree returns zero on success
or ax,ax
jnz failed
mov ax,[rejected_observed-data]
mov [guest_handle-data],ax
mov ax,[rejected_result-data]
mov [native_handle-data],ax
mov word [phase-data],5
call checkpoint
push word [owner-data]
api 2,147                        ; USER SetClipboardViewer
mov [viewer_next-data],ax
; A NULL HMEM16 is the documented original delayed-render request.  Native
; GetClipboardData must synchronously enter this Win16 WNDPROC with
; WM_RENDERFORMAT, which supplies CF_TEXT through the original WU32 bridge.
push word [owner-data]
api 2,137
test ax,ax
jz failed
api 2,139
test ax,ax
jz failed
push word 1
push word 0
api 2,141
test ax,ax
jnz failed
api 2,138
test ax,ax
jz failed
; Native replacement of the earlier eager guest format already sent a
; WM_DESTROYCLIPBOARD.  The delayed-owner assertion is about the one later
; replacement, so retain the original callback counters' phase baseline.
mov ax,[destroy_clipboard-data]
mov [destroy_clipboard_baseline-data],ax
mov ax,[draw_clipboard-data]
mov [draw_clipboard_baseline-data],ax
mov ax,4
call invoke_bridge
cmp word [render_format-data],1
jne failed
mov word [phase-data],6
call checkpoint
; Replacing the delayed owner from the native peer must retire the owner and
; notify the registered viewer through the same original callback bridge.
mov ax,5
call invoke_bridge
mov ax,[destroy_clipboard-data]
sub ax,[destroy_clipboard_baseline-data]
cmp ax,1
jne failed
mov ax,[draw_clipboard-data]
sub ax,[draw_clipboard_baseline-data]
or ax,ax
je failed
mov word [phase-data],7
call checkpoint
; Re-establish delayed ownership, then take the original owner-destruction
; path.  NT4 USER sends WM_RENDERALLFORMATS synchronously while an actual
; Clipboard-owner window is being destroyed.  Task cleanup is deliberately
; different: it patches departing procedures and must not call the task back.
push word [owner-data]
api 2,137                        ; USER OpenClipboard
test ax,ax
jz failed
api 2,139                        ; USER EmptyClipboard
test ax,ax
jz failed
push word 1
push word 0
api 2,141                        ; USER SetClipboardData, delayed CF_TEXT
test ax,ax
jnz failed
api 2,138                        ; USER CloseClipboard
test ax,ax
jz failed
mov word [phase-data],13         ; deferred owner exists before destruction
call checkpoint
push word [owner-data]
api 2,53                          ; USER DestroyWindow
test ax,ax
jz failed
cmp word [render_all-data],1
jne failed
; The callback records phase 14 while the owner remains callable.  Once this
; returns, the normal task exit has no owner left to render and must not enter
; the departing task again.
push32 module
api 1,514                        ; KERNEL FreeLibrary32W
test ax,ax
jz failed
push word [file-data]
api 1,81                         ; KERNEL _lclose
mov ax,0x4c00
int 0x21

%ifdef OWNER_TASK_EXIT
; Deliberately do not DestroyWindow. Original task/module rundown must
; withdraw this callback before destroying its delayed-Clipboard owner.
owner_task_exit:
push word [owner-data]
api 2,137
test ax,ax
jz failed
api 2,139
test ax,ax
jz failed
push word 1
push word 0
api 2,141
test ax,ax
jnz failed
api 2,138
test ax,ax
jz failed
mov ax,[owner-data]
mov [native_handle-data],ax       ; record the departing original guest HWND
mov word [phase-data],20
call checkpoint
push word [file-data]
api 1,81
mov ax,0x4c00
int 0x21
%endif

%ifdef CALLBACK_RETURN_ONLY
; Re-establish delayed ownership entirely inside the private test desktop,
; then use original USER's real owner-destruction path.  This is not a
; synthetic SendMessage: DestroyWindow must synchronously issue
; WM_RENDERALLFORMATS before it returns.
callback_return_only:
push word [owner-data]
api 2,137                          ; USER OpenClipboard
test ax,ax
jz failed
api 2,139                          ; USER EmptyClipboard
test ax,ax
jz failed
push word 1
push word 0
api 2,141                          ; delayed CF_TEXT
test ax,ax
jnz failed
api 2,138                          ; USER CloseClipboard
test ax,ax
jz failed
mov word [phase-data],13
call checkpoint
push word [owner-data]
api 2,53                           ; USER DestroyWindow
test ax,ax
jz failed
cmp word [render_all_entered-data],1
jne failed
cmp word [render_all-data],1
jne failed
mov ax,[render_handle-data]
mov [guest_handle-data],ax
mov word [phase-data],19           ; original DestroyWindow returned
call checkpoint
push32 module
api 1,514
test ax,ax
jz failed
push word [file-data]
api 1,81
mov ax,0x4c00
int 0x21
%endif

; Same original CallProc32W calling convention used by the generic callback
; witness. The bridge receives operation as its first DWORD and two zeros.
invoke_bridge:
push bp
mov bp,sp
push word 0
push ax
push word 0
push word 0
push word 0
push word 0
push32 native
push word 0
push word 0
push word 0
push word 3
api 1,517                        ; KERNEL CallProc32W
cmp sp,bp
jne fatal
cmp ax,1
jne failed
cmp dx,0x447a
jne failed
pop bp
ret

render_clipboard_text:
push word 0x2002                 ; GMEM_MOVEABLE | GMEM_DDESHARE
push word 0
push word deferred_payload_end-deferred_payload
api 1,15                         ; KERNEL GlobalAlloc
test ax,ax
jz .alloc_bad
mov [render_handle-data],ax
push ax
api 1,18                         ; KERNEL GlobalLock
mov di,ax
mov es,dx
or ax,dx
jz .lock_bad
mov si,deferred_payload-data
mov cx,deferred_payload_end-deferred_payload
cld
rep movsb
push word [render_handle-data]
api 1,19                         ; KERNEL GlobalUnlock
push word 1                      ; CF_TEXT
push word [render_handle-data]
api 2,141                        ; USER SetClipboardData while render is open
cmp ax,[render_handle-data]
jne .set_bad
ret
.alloc_bad:
mov word [native_handle-data],10
jmp .bad
.lock_bad:
mov word [native_handle-data],11
jmp .bad
.set_bad:
mov word [native_handle-data],12
.bad:
inc word [errors-data]
ret

window_proc:
push bp
mov bp,sp
push ds
push es
push si
push di
push bx
mov ds,ax
cmp word [bp+12],0x305           ; WM_RENDERFORMAT
je .render_format
cmp word [bp+12],0x306           ; WM_RENDERALLFORMATS
je .render_all
cmp word [bp+12],0x307           ; WM_DESTROYCLIPBOARD
je .destroy_clipboard
cmp word [bp+12],0x308           ; WM_DRAWCLIPBOARD
je .draw_clipboard
.default:
push word [bp+14]
push word [bp+12]
push word [bp+10]
push word [bp+8]
push word [bp+6]
api 2,107                        ; USER DefWindowProc
jmp .return
.render_format:
cmp word [bp+10],1
jne .bad
call render_clipboard_text
inc word [render_format-data]
xor ax,ax
jmp .value
.render_all:
; WM_RENDERALLFORMATS is unlike WM_RENDERFORMAT: the owner must open the
; Clipboard, verify that it still owns it, render all delayed formats, then
; close it.  Keep this independently-authored Win16 witness faithful to the
; original USER contract rather than relying on a host-side lock accident.
; Preserve an on-disk stage witness before each original USER transition.  A
; modern-host deadlock must identify the transition that did not return;
; these writes have no bearing on the product callback protocol.
inc word [render_all_entered-data]
mov word [phase-data],14           ; entered WM_RENDERALLFORMATS
call checkpoint
push word [owner-data]
api 2,137                        ; USER OpenClipboard
test ax,ax
jz .render_all_open_bad
mov word [phase-data],15           ; OpenClipboard returned
call checkpoint
api 2,140                        ; USER GetClipboardOwner
cmp ax,[owner-data]
jne .render_all_owner_bad
mov word [phase-data],16           ; owner identity returned
call checkpoint
call render_clipboard_text
mov word [phase-data],17           ; delayed format rendered
call checkpoint
.render_all_close:
api 2,138                        ; USER CloseClipboard
test ax,ax
jz .render_all_close_bad
mov word [phase-data],18           ; CloseClipboard returned
call checkpoint
.render_all_done:
inc word [render_all-data]
xor ax,ax
jmp .value
.render_all_owner_bad:
api 2,138
jmp .render_all_bad
.render_all_open_bad:
mov word [native_handle-data],1
jmp .render_all_bad
.render_all_close_bad:
mov word [native_handle-data],2
.render_all_bad:
inc word [errors-data]
xor ax,ax
jmp .value
.destroy_clipboard:
inc word [destroy_clipboard-data]
xor ax,ax
jmp .value
.draw_clipboard:
inc word [draw_clipboard-data]
xor ax,ax
jmp .value
.bad:
inc word [errors-data]
xor ax,ax
.value:
mov dx,0x447a
.return:
; Win16 USER uses Pascal callee cleanup. Each callback must restore the
; five saved words before RETF 10; a stray argument to a zero-argument API
; must fail here rather than masquerade as a product callback-return fault.
mov bx,bp
sub bx,10
cmp sp,bx
jne fatal
pop bx
pop di
pop si
pop es
pop ds
pop bp
retf 10

checkpoint:
push word [file-data]
push ds
push word report-data
push word report_end-report
api 1,86                         ; KERNEL _lwrite
cmp ax,report_end-report
jne fatal
ret

failed:
inc word [errors-data]
call checkpoint
push word [file-data]
api 1,81
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
report: db 'CLP1'
phase: dw 0
errors: dw 0
guest_handle: dw 0
native_handle: dw 0
report_end:
instance: dw 0
file: dw 0
owner: dw 0
viewer_next: dw 0
procedure: dd 0
module: dd 0
native: dd 0
render_handle: dw 0
render_format: dw 0
render_all: dw 0
render_all_entered: dw 0
destroy_clipboard: dw 0
draw_clipboard: dw 0
destroy_clipboard_baseline: dw 0
draw_clipboard_baseline: dw 0
wndclass:
dw 0
dd 0
dw 0,0,0,0,0,0
dd 0
dw classname-data,0
classname: db 'T460_CLIPBOARD',0
title: db 'Clipboard probe owner',0
library: db 'GENHOST.DLL',0
export: db 'ClipboardBridge',0
filename: db 'CLIPPRB.BIN',0
guest_payload: db 'guest clipboard',0
guest_payload_end:
native_payload: db 'native clipboard',0
native_payload_end:
rejected_payload: db 'rejected clipboard',0
rejected_payload_end:
deferred_payload: db 'deferred clipboard',0
deferred_payload_end:
rejected_handle: dw 0
rejected_result: dw 0
rejected_observed: dw 0
data_end:
image_end:
