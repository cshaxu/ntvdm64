; Independently authored disposable NE probe. Original guest images unchanged.
; READY/GO file handshake permits a native peer to create a window after
; guest initialization, without a USER message refreshing the guest graph.
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
resident: db 8,'DISCOVER'
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
push word ready_name-data
push word 0
api 3                         ; _lcreat READY.DAT
cmp ax,-1
je fatal
push ax
api 4                         ; _lclose
.wait:
push ds
push word go_name-data
push word 0
api 5                         ; _lopen GO.DAT; no USER call while waiting
cmp ax,-1
je .wait
push ax
api 6                         ; _lclose
push ds
push word cls-data
push ds
push word title-data
api 7                         ; FindWindow
mov [found-data],ax
test ax,ax
jz failed
call query_class
push cs
push word enum_proc-code
push word [instance-data]
api 8                         ; MakeProcInstance
mov [proc-data],ax
mov [proc-data+2],dx
or ax,dx
jz failed
push dx
push word [proc-data]
push word 0x1234
push word 0x5678
api 9                         ; EnumWindows (callback stops at target)
push word [proc-data+2]
push word [proc-data]
api 10                        ; FreeProcInstance
mov word [freed-data],1
push cs
push word property_proc-code
push word [instance-data]
api 15                        ; MakeProcInstance for original EnumProps callback
mov [proc-data],ax
mov [proc-data+2],dx
or ax,dx
jz failed
push word [found-data]
push dx
push word [proc-data]
api 16                        ; EnumProps -> W32EnumPropsFunc -> CallBack16
mov [property_result-data],ax
push word [proc-data+2]
push word [proc-data]
api 17                        ; FreeProcInstance
inc word [freed-data]
jmp finish
failed:
inc word [errors-data]
finish:
push ds
push word result_name-data
push word 0
api 11                        ; _lcreat RESULT.BIN
cmp ax,-1
je fatal
mov [file-data],ax
push ax
push ds
push word report-data
push word report_end-report
api 12                        ; _lwrite
cmp ax,report_end-report
jne fatal
push word [file-data]
api 13                        ; _lclose
cmp word [errors-data],0
jne fatal
cmp word [matched-data],1
jne fatal
cmp word [queries-data],3
jne fatal
cmp word [properties-data],1
jne fatal
cmp word [property_result-data],-7
jne fatal
mov ax,0x4c00
int 0x21
fatal:
mov ax,0x4c01
int 0x21

query_class:
push ax
push ds
push word query-data
push word 16
api 14                        ; GetClassName: original USER16 direct reader
cmp ax,6
jne .bad
push si
push di
push cx
mov si,query-data
mov di,expected-data
mov cx,6
.compare:
lodsb
or al,0x20
cmp al,[di]
jne .mismatch
inc di
loop .compare
inc word [queries-data]
jmp .done
.mismatch: inc word [errors-data]
.done:
pop cx
pop di
pop si
ret
.bad:
inc word [errors-data]
ret

enum_proc:
push bp
mov bp,sp
push ds
mov ds,ax                     ; MakeProcInstance supplies instance in AX
push bx
push si
push di
push es
cmp word [bp+6],0x5678
jne .bad
cmp word [bp+8],0x1234
jne .bad
mov ax,[bp+10]                ; Pascal HWND, DWORD lParam
cmp ax,[found-data]
jne .next
inc word [matched-data]
call query_class
xor ax,ax                     ; original enumeration early-stop contract
jmp .return
.bad:
inc word [errors-data]
.next: mov ax,1
.return:
xor dx,dx
pop es
pop di
pop si
pop bx
pop ds
pop bp
retf 6
property_proc:
push bp
mov bp,sp
push ds
mov ds,ax
push bx
push si
push di
push es
cmp word [bp+6],0x4567        ; property data, far string, HWND (Pascal)
jne .bad
mov ax,[bp+12]
cmp ax,[found-data]
jne .bad
call query_class
mov es,[bp+10]
mov si,[bp+8]
mov di,property_name-data
push cx
mov cx,9
.compare:
mov al,[es:si]
cmp al,[di]
jne .mismatch
inc si
inc di
loop .compare
pop cx
inc word [properties-data]
jmp .return
.mismatch:
pop cx
.bad:
inc word [errors-data]
.return:
mov ax,-7
cwd
pop es
pop di
pop si
pop bx
pop ds
pop bp
retf 8
code_end:
dw 17
%macro fixup 3
db 3,1
dw ..@call_%1-code+1,%2,%3
%endmacro
fixup 1,1,91
fixup 2,2,5
fixup 3,1,83
fixup 4,1,81
fixup 5,1,85
fixup 6,1,81
fixup 7,2,50
fixup 8,1,51
fixup 9,2,54
fixup 10,1,52
fixup 11,1,83
fixup 12,1,86
fixup 13,1,81
fixup 14,2,58
fixup 15,1,51
fixup 16,2,27
fixup 17,1,52
align 16,db 0
data:
times 32 db 0
report: db 'DSC1'
errors: dw 0
found: dw 0
matched: dw 0
queries: dw 0
freed: dw 0
properties: dw 0
property_result: dw 0
report_end:
instance: dw 0
file: dw 0
proc: dd 0
query: times 16 db 0
cls: db 'STATIC',0
expected: db 'static'
property_name: db 'T447PROP',0
title: db 'S2 late native discovery',0
ready_name: db 'READY.DAT',0
go_name: db 'GO.DAT',0
result_name: db 'RESULT.BIN',0
data_end:
image_end:
