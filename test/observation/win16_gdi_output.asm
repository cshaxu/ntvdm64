; Actual Win16 GDI output through the XP interpreted thunk table.
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
resident: db 8,'GDIPROBE'
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
push di
api 2                         ; InitApp
test ax,ax
jz fatal
push word 0
api 3                         ; GetDC(NULL)
test ax,ax
jz fatal
mov [dc-data],ax
call check_font_data
push word 0
push word [dc-data]
api 4                         ; ReleaseDC
push ds
push word palette-data
api 5                         ; CreatePalette
test ax,ax
jz fatal
mov [hp-data],ax
; start, count, expected return, expected first entry (little-endian RGB+flags)
%macro query 5
mov dword [palette_bytes-data],0xeeeeeeee
mov dword [palette_bytes-data+4],0xeeeeeeee
mov dword [palette_bytes-data+8],0xeeeeeeee
push word [hp-data]
push word %2
push word %3
push ds
push word palette_bytes-data
api %1
cmp ax,%4
jne fatal
cmp dword [palette_bytes-data],%5
jne fatal
cmp byte [palette_bytes-data+%4*4],0xee
jne fatal
%endmacro
query 6,1,4,1,0x00060504
query 7,0,2,2,0x00030201
cmp dword [palette_bytes-data+4],0x00060504
jne fatal
query 8,2,4,0,0xeeeeeeee
push word [hp-data]
api 9                         ; DeleteObject
mov word [hp-data],0
query 10,0,1,0,0xeeeeeeee
push ds
push word success_file-data
push word 0
api 11                        ; _lcreat
cmp ax,-1
je fatal
push ax
api 12                        ; _lclose
mov ax,0x4c00
int 0x21
fatal:
mov ax,0x4c01
int 0x21
; Exercise the interpreted GDI ordinal with a null pointer, then a guest buffer.
check_font_data:
push ds
push word logfont-data
api 20                        ; CreateFontIndirect
test ax,ax
jz fatal
mov [font-data],ax
push word [dc-data]
push ax
api 21                        ; SelectObject
test ax,ax
jz fatal
mov [old_font-data],ax
push word [dc-data]
push dword 0x64616568         ; TrueType 'head' table
push dword 0
push dword 0
push dword 0
api 22                        ; GetFontData: size query
test dx,dx
jnz fatal
cmp ax,16
jb fatal
push word [dc-data]
push dword 0x64616568
push dword 0
push ds
push word table_bytes-data
push dword 16
api 23                        ; GetFontData: read table header
test dx,dx
jnz fatal
cmp ax,16
jne fatal
cmp dword [table_bytes-data+12],0xf53c0f5f
jne fatal
cmp byte [table_bytes-data+16],0xa5
jne fatal
push word [dc-data]
push word [old_font-data]
api 24                        ; SelectObject
push word [font-data]
api 25                        ; DeleteObject
ret
code_end:
dw 18
%macro fixup 3
db 3,1
dw ..@call_%1-code+1,%2,%3
%endmacro
fixup 1,1,91
fixup 2,2,5
fixup 3,2,66
fixup 4,2,68
fixup 5,3,360
fixup 6,3,363
fixup 7,3,363
fixup 8,3,363
fixup 9,3,69
fixup 10,3,363
fixup 11,1,83
fixup 12,1,81
fixup 20,3,57
fixup 21,3,45
fixup 22,3,311
fixup 23,3,311
fixup 24,3,45
fixup 25,3,69
align 16,db 0
data:
times 32 db 0
dc: dw 0
hp: dw 0
success_file: db 'GDIOK.DAT',0
palette: dw 0x300,2
db 1,2,3,0,4,5,6,0
palette_bytes: times 16 db 0xee
font: dw 0
old_font: dw 0
logfont: dw 24,0,0,0,400
db 0,0,0,0,7,0,0,0
db 'Arial',0
times 26 db 0
table_bytes: times 17 db 0xa5
data_end:
image_end:
