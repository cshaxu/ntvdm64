; Independently authored test NE executable, never a replacement guest.
; Calls the unchanged KERNEL APIs whose ordinals are in kernel31/kernel.def.
; Outputs three fixed 256-byte NUL-terminated fields, then a completion file.
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
dw 0x0302,2,0x100,0x1000  ; Windows EXE, multiple data, auto DS2
dw 0,1,0,2                ; entry CS1:0, automatic SS2
dw 2,2,0                  ; segments, modules, nonresident bytes
dw segments-ne,resources-ne,resident-ne,modules-ne,imports-ne
dd 0
dw 0,4,0
db 2,0                    ; Windows target
dw 0,0,0,0x030a
segments:
dw (code-$$)/16,code_end-code,0x0150,code_end-code
dw (data-$$)/16,data_end-data,0x0c51,0x2000
resources: dw 0,0
resident: db 4,'DIRP'
dw 0
db 0
modules: dw kernel-imports,user-imports
imports:
db 0                       ; Offset zero is the empty import-name sentinel.
kernel: db 6,'KERNEL'
user: db 4,'USER'
entries: db 0
entries_end:
align 16,db 0
code:
init_call: db 0x9a
dw 0xffff,0
push di
init_app_call: db 0x9a
dw 0xffff,0
push ds
push word report_name-data
push word 0
create_call: db 0x9a
dw 0xffff,0
cmp ax,-1
je failed
mov [file-data],ax
push ds
push word windows-data
push word 256
windows_call: db 0x9a
dw 0xffff,0
push ds
push word system-data
push word 256
system_call: db 0x9a
dw 0xffff,0
push ds
push word module_name-data
handle_call: db 0x9a
dw 0xffff,0
push ax
push ds
push word module-data
push word 256
module_call: db 0x9a
dw 0xffff,0
push word [file-data]
push ds
push word windows-data
push word 768
write_call: db 0x9a
dw 0xffff,0
cmp ax,768
jne failed_close
push word [file-data]
close_call: db 0x9a
dw 0xffff,0
push ds
push word done_name-data
push word 0
done_create_call: db 0x9a
dw 0xffff,0
cmp ax,-1
je failed
push ax
done_close_call: db 0x9a
dw 0xffff,0
mov ax,0x4c00
int 0x21
failed_close:
push word [file-data]
failed_close_call: db 0x9a
dw 0xffff,0
failed:
mov ax,0x4c01
int 0x21
code_end:
dw 12
%macro fixup 2-3 1
db 3,1
dw %1-code+1,%3,%2
%endmacro
fixup init_call,91
fixup init_app_call,5,2
fixup windows_call,134
fixup system_call,135
fixup handle_call,47
fixup module_call,49
fixup create_call,83
fixup write_call,86
fixup close_call,81
fixup done_create_call,83
fixup done_close_call,81
fixup failed_close_call,81
align 16,db 0
data:
; Win16 local-heap reserved words must not overlap strings.
times 32 db 0
windows: times 256 db 0
system: times 256 db 0
module: times 256 db 0
file: dw 0
module_name: db 'KERNEL',0
report_name: db 'Z:\tests\DIRP.BIN',0
done_name: db 'Z:\tests\DIRP.DON',0
data_end:
image_end:
