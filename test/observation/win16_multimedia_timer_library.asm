; Independently authored Win16 multimedia test; never replacement guest media.
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
dw 0x8001,2,0,0
dw 0,1,0,0
dw 2,1,0
dw segments-ne,resources-ne,resident-ne,modules-ne,imports-ne
dd 0
dw 0,4,0
db 2,0
dw 0,0,0,0x030a
segments:
dw (code-$$)/16,code_end-code,0x0140,code_end-code
dw (data-$$)/16,data_end-data,0x0041,data_end-data
resources: dw 0,0
resident: db 8,'MMTPROBE'
dw 0
db 3,'WEP'
dw 5
db 0
modules: dw mmsystem-imports
imports: db 0
mmsystem: db 8,'MMSYSTEM'
entries:
db 5,1
db 1
dw callback-code
db 1
dw query-code
db 1
dw start-code
db 1
dw stop-code
db 1
dw wep-code
db 0
entries_end:
%assign calls 0
%macro api 2
%assign calls calls+1
..@call_%[calls]: db 0x9a
dw 0xffff,0
%xdefine module_%[calls] %1
%xdefine ordinal_%[calls] %2
%endmacro
%assign selectors 0
%macro own_ds 0
push ds
db 0xb8
%assign selectors selectors+1
..@selector_%[selectors]: dw 0xffff
mov ds,ax
%endmacro
align 16,db 0
code:
mov ax,1
retf
callback:
push bp
mov bp,sp
own_ds
inc word [count-data]
cmp word [ss:bp+14],0x3344
jne .bad
cmp word [ss:bp+16],0x1122
je .done
.bad:
inc word [bad-data]
.done:
pop ds
pop bp
retf 16
query:
own_ds
mov ax,[count-data]
mov dx,[bad-data]
pop ds
retf
start:
own_ds
mov word [count-data],0
mov word [bad-data],0
push word 20
push word 1
push cs
push word callback-code
push word 0x1122
push word 0x3344
push word 1
api 1,602
mov [timer-data],ax
pop ds
retf
stop:
own_ds
push word [timer-data]
api 1,603
pop ds
retf
wep:
mov ax,1
retf 2
code_end:
dw calls+selectors
%assign i 1
%rep calls
db 3,1
dw ..@call_%[i]-code+1,module_%[i],ordinal_%[i]
%assign i i+1
%endrep
%assign i 1
%rep selectors
db 2,0
dw ..@selector_%[i]-code
 db 2,0
 dw 0
%assign i i+1
%endrep
align 16,db 0
data:
times 32 db 0 ; Reserved Win16 automatic-data prefix.
count: dw 0
bad: dw 0
timer: dw 0
data_end:
image_end:
