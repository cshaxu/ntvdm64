; T479 S6 isolated input-route witness, never a release/runtime dependency.
; ABI: original MS-DOS6 VMM.INC; Microsoft VxD DDK chapters33/36.
; Only records boundaries using the existing host diagnostic BOP log.
; No keyboard port reads, event consumption, translation or host task delivery.
bits 32
%macro vmm_call 1
 int 0x20
 dd 0x10000 + %1
%endmacro
segment _LTEXT public align=16 class=CODE use32
global control
control:
 pushfd
 pushad
 mov edx,eax
 mov eax,0x53490020
 call record_vm
 popad
 popfd
 cmp eax,1
 jne .done
 pushad
 mov eax,0x0d
 mov esi,pm_api
 vmm_call 0x92
 jc .pm_absent
 mov [previous_pm],esi
 or dword [installed],1
.pm_absent:
 mov eax,9
 mov esi,irq1
 vmm_call 0x41
 jc .irq_absent
 or dword [installed],2
.irq_absent:
 mov edx,[installed]
 mov eax,0x53490000
 call record_vm
 popad
.done:
 clc
 ret
; Diagnostic records preserve every caller register and EFLAGS. The low VM
; handle is BX and high VM handle is CX; DX carries the observation value.
record_vm:
 pushfd
 pushad
 mov ecx,ebx
 shr ecx,16
 db 0xc4,0xc4,0xb1
 popad
 popfd
 ret
irq1:
 pushfd
 pushad
 cmp dword [irq_count],128
 jae .chain
 inc dword [irq_count]
 mov edx,[irq_count]
 mov eax,0x53490009
 call record_vm
.chain:
 popad
 popfd
 stc                       ; original IRQ chain always owns consumption/EOI
 ret
pm_api:
 pushfd
 pushad
 cmp dword [pm_count],128
 jae .chain
 inc dword [pm_count]
 mov edx,[ebp+28]           ; client AX function; EBX is calling VM, not target
 mov eax,0x53490010
 call record_vm
 mov ebx,[ebp+16]           ; record client EBX separately; no target inference
 mov edx,[ebp+24]           ; client CX event payload
 mov eax,0x53490011
 call record_vm
 mov edx,[ebp+20]           ; client DX flags/payload
 mov eax,0x53490012
 call record_vm
.chain:
 popad
 popfd
 jmp [previous_pm]
segment _LDATA public align=16 class=DATA use32
global input_route_ddb
input_route_ddb:
 dd 0
 dw 0x300,0
 db 0,1
 dw 0
 db 'VMINPUT '
 dd 0x90000000
 dd control,0,0,0,0,0,0,0
previous_pm: dd 0
installed: dd 0
irq_count: dd 0
pm_count: dd 0
