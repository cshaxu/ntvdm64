
; Win95 puts a toolbar in the DOS window's parent client area. Locate the
; actual text child rather than assuming the Win3.1 parent geometry.
mouse_find_text:
push word [active-data]
push word 5                       ; GW_CHILD
api 2,262
.next:
test ax,ax
jz fatal
mov [mouse_hwnd-data],ax
push ax
push ds
push word mouse_class-data
push word 64
api 2,58                        ; GetClassName
cmp dword [mouse_class-data],0x47797474 ; ttyG
jne .other
cmp word [mouse_class-data+4],0x6172   ; ra
jne .other
cmp word [mouse_class-data+6],0x0062   ; b and terminator
je .found
.other:
push word [mouse_hwnd-data]
push word 2                       ; GW_HWNDNEXT
api 2,262
jmp .next
.found:
ret
