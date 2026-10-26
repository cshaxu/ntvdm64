; Noninteractive Win16 witness for the already-loaded mouse bridge.
; Calls its real DPMI thunk with guest test packets; no host input or UI control.
; Reads the resulting original USER cursor/button state and writes M31WIN.BIN.
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
resident: db 6,'M31WIN'
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
%macro check 2
cmp %1,%2
je %%ok
jmp failed
%%ok:
%endmacro
%macro coordinate 2
; USER versions round the 0..FFFF normalized endpoint at most one pixel apart.
mov ax,%1
sub ax,%2-1
cmp ax,1
jbe %%ok
jmp failed
%%ok:
%endmacro
align 16,db 0
code:
api 1,91                         ; InitTask
test ax,ax
jz fatal
mov [instance-data],di
push ds
push word filename-data
push word 0
api 1,83                         ; _lcreat
cmp ax,-1
je fatal
mov [file-data],ax
mov word [phase-data],1
call checkpoint
push word [instance-data]
api 2,5                          ; InitApp
test ax,ax
jz failed
api 1,3                          ; GetVersion
mov [version-data],ax
api 1,132                        ; GetWinFlags
mov [winflags-data],ax
mov word [phase-data],2
call checkpoint
push ds
push word module_name-data
api 1,47                         ; GetModuleHandle("MOUSE")
test ax,ax
jz failed
push ax
push word 0
push word 2
api 1,50                         ; GetProcAddress(Enable ordinal 2)
mov bx,ax
mov es,dx
check byte [es:bx],0xb8          ; loader-patched automatic DS prolog
mov ax,[es:bx+1]
mov [driver_ds-data],ax
mov es,ax
check byte [es:44],1             ; allocated
check byte [es:45],1             ; installed
check byte [es:46],1             ; enabled
mov ax,[es:40]
mov [thunk-data],ax
mov ax,[es:42]
mov [thunk+2-data],ax
mov word [phase-data],3
call checkpoint

mov word [packet+24-data],100
mov word [packet+20-data],80
mov word [packet+28-data],1
call invoke
call cursor
mov ax,[point-data]
mov [first_x-data],ax
mov ax,[point+2-data]
mov [first_y-data],ax
coordinate [first_x-data],100
coordinate [first_y-data],80
mov word [phase-data],4
call checkpoint

mov word [packet+24-data],520
mov word [packet+20-data],400
mov word [packet+28-data],1
call invoke
call cursor
mov ax,[point-data]
mov [second_x-data],ax
mov ax,[point+2-data]
mov [second_y-data],ax
coordinate [second_x-data],520
coordinate [second_y-data],400
mov word [phase-data],5
call checkpoint

mov word [packet+28-data],2       ; left down/up
call invoke
push word 1
api 2,249                        ; GetAsyncKeyState(VK_LBUTTON)
mov [left_down-data],ax
test ax,0x8000
jz failed
mov word [packet+28-data],4
call invoke
push word 1
api 2,249
mov [left_up-data],ax
test ax,0x8000
jnz failed
mov word [phase-data],6
call checkpoint
mov word [packet+28-data],8       ; right down/up
call invoke
push word 2
api 2,249
mov [right_down-data],ax
test ax,0x8000
jz failed
mov word [packet+28-data],16
call invoke
push word 2
api 2,249
mov [right_up-data],ax
test ax,0x8000
jnz failed
mov word [phase-data],7
call checkpoint
push word [file-data]
api 1,81                         ; _lclose
mov ax,0x4c00
int 0x21

invoke:
mov ax,[thunk-data]
mov [packet+42-data],ax
mov ax,[thunk+2-data]
mov [packet+44-data],ax
mov word [packet+46-data],0
mov word [packet+48-data],0
mov word [packet+32-data],0x0200
push ds
pop es
mov di,packet-data
xor cx,cx
xor bx,bx
mov ax,0x0301                   ; call actual allocated real->protected thunk
int 0x31
jc failed
inc word [events-data]
ret
cursor:
push ds
push word point-data
api 2,17                         ; original USER GetCursorPos
ret
checkpoint:
push word [file-data]
push ds
push word report-data
push word report_end-report
api 1,86                         ; _lwrite, append a complete phase record
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
report: db 'M31W'
phase: dw 0
errors: dw 0
version: dw 0
winflags: dw 0
driver_ds: dw 0
thunk: dd 0
events: dw 0
first_x: dw 0
first_y: dw 0
second_x: dw 0
second_y: dw 0
left_down: dw 0
left_up: dw 0
right_down: dw 0
right_up: dw 0
report_end:
file: dw 0
instance: dw 0
point: dw 0,0
packet: times 50 db 0
module_name: db 'MOUSE',0
filename: db 'O:\W31\PATCH\M31WIN.BIN',0
data_end:
image_end:
