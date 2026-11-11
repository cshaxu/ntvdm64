; Isolated S6 VKD ownership witness. Never a runtime dependency.
; ABI facts: Microsoft DDK chapter36 and OpenNT-4.5 OS2 thunk vkd.inc.
bits 32
%macro vkd_call 1
 int 0x20
 dd 0x000d0000 + %1
%endmacro
segment _LTEXT public align=16 class=CODE use32
global control,api
control:
 clc
 ret
api:
 pushad
 mov eax,[ebp+28]
 cmp ax,1
 je .single
 cmp ax,2
 je .overlap
 cmp ax,3
 je .status
 mov dword [ebp+28],87
 jmp .done
.single:
 mov dword [callbacks],0
 mov dword [outcomes],0
 mov dword [source_a],0x001071 ; OEM q, scan10, no shift
 mov eax,source_a
 mov edx,1
 call paste
 mov [ebp+28],eax
 ; A successful Start_Paste promises to copy, not borrow this buffer.
 mov byte [source_a],'z'
 jmp .done
.overlap:
 mov dword [callbacks],0
 mov dword [outcomes],0
 mov dword [source_a],0x001071
 mov eax,source_a
 mov edx,1
 call paste
 mov [ebp+28],eax
 test eax,eax
 jnz .done
 mov eax,source_b
 mov edx,2
 call paste
 mov [ebp+28],eax
 jmp .done
.status:
 mov eax,[callbacks]
 mov [ebp+28],eax
 mov eax,[outcomes]
 mov [ebp+24],eax
.done:
 popad
 ret
paste:
 mov ecx,1
 mov esi,completed
 vkd_call 0x0a
 mov eax,0
 jnc .ok
 mov eax,8
.ok:
 ret
completed:
 inc dword [callbacks]
 ; Each byte: upper nibble caller reference, lower nibble completion reason.
 and eax,15
 shl edx,4
 or eax,edx
 shl dword [outcomes],8
 or [outcomes],eax
 ret
segment _LDATA public align=16 class=DATA use32
global paste_route_ddb
paste_route_ddb:
 dd 0
 dw 0x300,0x7f83
 db 0,1
 dw 0
 db 'PASTETST'
 dd 0x90000000
 dd control,api,api,0,0,0,0,0
source_a: dd 0x001071
source_b: dd 0x001372 ; OEM r, scan13, no shift
callbacks: dd 0
outcomes: dd 0
