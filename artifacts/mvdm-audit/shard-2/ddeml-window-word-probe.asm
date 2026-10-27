; Independently authored Win16 probe. Does not modify original guest code.
; Static DDEML import; initializes, reinitializes, and uninitializes one instance.
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
resident: db 7,'DDINIT1'
dw 0
db 0
modules: dw kernel-imports,ddeml-imports,user-imports
imports:
db 0
kernel: db 6,'KERNEL'
ddeml: db 5,'DDEML'
user: db 4,'USER'
entries: db 0
entries_end:
%macro api 1
..@call_%1: db 0x9a
dw 0xffff,0
%endmacro
%macro dde_args 1
 push ds
 push word instance-data
 push cs
 push word dde_callback-code
 push word 0
 push word 0x0010 ; APPCMD_CLIENTONLY
 push word 0
 push word %1
%endmacro
%macro mark 1
 push ax
 push bx
 push cx
 push dx
 mov byte [stage_value-data],%1
 mov dx,stage_name-data
 xor cx,cx
 mov ah,0x3c
 int 0x21
 jc %%done
 mov bx,ax
 mov dx,stage_value-data
 mov cx,1
 mov ah,0x40
 int 0x21
 mov ah,0x3e
 int 0x21
%%done:
 pop dx
 pop cx
 pop bx
 pop ax
%endmacro
%macro capture 0
 push ax
 push bx
 push cx
 push dx
 mov dx,pre_name-data
 xor cx,cx
 mov ah,0x3c
 int 0x21
 jc %%done
 mov bx,ax
 mov dx,report_start-data
 mov cx,report_end-report_start
 mov ah,0x40
 int 0x21
 mov ah,0x3e
 int 0x21
%%done:
 pop dx
 pop cx
 pop bx
 pop ax
%endmacro
align 16,db 0
code:
api 1
 test ax,ax
 jz fatal
 mov [task_instance-data],di
 push di
 api 2
 mark 1
 test ax,ax
 jz fatal
 push ds
 push word module_name-data
 api 3
 mov [module_handle-data],ax
 push ax
 push ds
 push word module_path-data
 push word 260
 api 4
 mov [path_length-data],ax
 push cs
 push word dde_callback-code
 push word [task_instance-data]
 api 5
 mark 2
 mov [callback_ptr-data],ax
 mov [callback_ptr+2-data],dx
 or ax,dx
 jz report
%ifdef SELF_CALLBACK_CONTROL
%rep 14
 push word 0
%endrep
 call far [callback_ptr-data]
 mark 11
%endif
 push ds
 push word instance-data
 push word [callback_ptr+2-data]
 push word [callback_ptr-data]
 push word 0
 push word 0
 push word 0
 push word 0
 api 6
 mark 3
 mov [init_result-data],ax
 test ax,ax
 jnz report
 mov ax,[instance-data]
 mov [init_id_lo-data],ax
 mov ax,[instance+2-data]
 mov [init_id_hi-data],ax
 push word [instance+2-data]
 push word [instance-data]
 push ds
 push word service_name-data
 push word 1004
 api 7
 mark 4
 mov [service_lo-data],ax
 mov [service_hi-data],dx
 or ax,dx
 jz uninit
 push ds
 push word dmg_class-data
 push word 0
 push word 0
 api 16
 mov [register_count-data],ax
 test ax,ax
 jz no_window
 push ax
 push word 0
 api 17
 mov [unregister_count-data],ax
no_window:
%ifdef NO_REGISTER_CONTROL
 jmp free_only
%endif
 push word [instance+2-data]
 push word [instance-data]
 push word [service_hi-data]
 push word [service_lo-data]
 push word 0
 push word 0
 push word 1
 mark 9
 push ax
 mov ax,[callback_ptr-data]
 mov [first_type-data],ax
 mov ax,[callback_ptr+2-data]
 mov [last_type-data],ax
 pop ax
 capture
 mov word [first_type-data],0
 mov word [last_type-data],0
 api 8
 mark 5
 mov [register_lo-data],ax
 mov [register_hi-data],dx
 mov ax,[callback_count-data]
 mov [register_count-data],ax
 push word [instance+2-data]
 push word [instance-data]
 push word [service_hi-data]
 push word [service_lo-data]
 push word 0
 push word 0
 push word 2
 api 9
 mark 6
 mov [unregister_lo-data],ax
 mov [unregister_hi-data],dx
 mov ax,[callback_count-data]
 mov [unregister_count-data],ax
free_only:
 push word [instance+2-data]
 push word [instance-data]
 push word [service_hi-data]
 push word [service_lo-data]
 api 10
 mov [free_result-data],ax
uninit:
 push word [instance+2-data]
 push word [instance-data]
 api 11
 mark 7
 mov [uninit_result-data],ax
report:
 mov ax,[callback_ptr-data]
 or ax,[callback_ptr+2-data]
 jz no_free
 push word [callback_ptr+2-data]
 push word [callback_ptr-data]
 api 12
no_free:
 push ds
 push word result_name-data
 push word 0
 api 13
 cmp ax,-1
 je fatal
 mov [file-data],ax
 push ax
 push ds
 push word report_start-data
 push word report_end-report_start
 api 14
 cmp ax,report_end-report_start
 jne fatal
 push word [file-data]
 api 15
 mov ax,0x4c00
 int 0x21
fatal:
 mov ax,0x4c01
 int 0x21
dde_callback:
 push bp
 mov bp,sp
 inc word [callback_count-data]
 mov ax,[ss:bp+32]
 cmp word [first_type-data],0
 jne .has_first
 mov [first_type-data],ax
.has_first:
 mov [last_type-data],ax
 xor ax,ax
 xor dx,dx
 pop bp
 retf 28
code_end:
dw 17
%macro fixup 3
db 3,1
dw ..@call_%1-code+1,%2,%3
%endmacro
fixup 1,1,91
fixup 2,1,5
fixup 3,1,47
fixup 4,1,49
fixup 5,1,51
fixup 6,2,2
fixup 7,2,21
fixup 8,2,27
fixup 9,2,27
fixup 10,2,22
fixup 11,2,3
fixup 12,1,52
fixup 13,1,83
fixup 14,1,86
fixup 15,1,81
fixup 16,3,50
fixup 17,3,133
align 16,db 0
data:
times 32 db 0
report_start: db 'DDR1'
module_handle: dw 0
path_length: dw 0
init_result: dw 0xffff
init_id_lo: dw 0
init_id_hi: dw 0
service_lo: dw 0
service_hi: dw 0
register_lo: dw 0xffff
register_hi: dw 0xffff
unregister_lo: dw 0xffff
unregister_hi: dw 0xffff
free_result: dw 0xffff
uninit_result: dw 0xffff
callback_count: dw 0
register_count: dw 0xffff
unregister_count: dw 0xffff
first_type: dw 0
last_type: dw 0
module_path: times 260 db 0
report_end:
instance: dd 0
callback_ptr: dd 0
task_instance: dw 0
file: dw 0
module_name: db 'DDEML',0
pre_name: db 'DPRE.BIN',0
stage_name: db 'DSTAGE.BIN',0
stage_value: db 0
dmg_class: db 'DMGClass',0
service_name: db 'S10DDEPROBE',0
result_name: db 'DREG.BIN',0
data_end:
image_end:
