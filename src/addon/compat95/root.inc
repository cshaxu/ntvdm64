; Independent root receiver, appended to preserve the IOS vector-slot ABI.
root_path db 'O:\WINDOWS\SYSTEM.DAT',0
 times 128-($-root_path) db 0
root_length dw 22
root_suffix db '\SYSTEM.DAT',0
root_dispatch:
 pushf
 cmp ax,1613h
 jne .chain
 cmp cx,[cs:root_length]
 jb .chain
 push ds
 push si
 push di
 push cs
 pop ds
 mov si,root_path
 mov cx,[root_length]
 cld
 rep movsb
 pop di
 pop si
 pop ds
 mov cx,[cs:root_length]
 xor ax,ax
 popf
 iret
.chain:
 popf
%ifndef COMPAT95_NO_LOGO
 jmp logo_dispatch
%else
 jmp multiplex
%endif

; Invalid /ROOT fails before any vectors change; absent option keeps default.
root_parse_options:
 pushad
 push ds
 push es
 les bx,[request]
 lds si,[es:bx+18]
 mov cx,255
.scan:
 lodsb
 test al,al
 jz .ok
 cmp al,13
 je .ok
 cmp al,'/'
 jne .next
 cmp cx,6
 jb .bad
 mov eax,[si]
 or eax,20202020h
 cmp eax,'root'
 jne .next
 cmp byte [si+4],'='
 jne .next
 add si,5
 push cs
 pop es
 mov di,root_path
 mov cx,110
.copy:
 lodsb
 cmp al,13
 je .end
 test al,al
 jz .end
 cmp al,' '
 je .end
 cmp al,9
 je .end
 cmp al,21h
 jb .bad
 cmp al,7fh
 jae .bad
 cmp al,'"'
 je .bad
 stosb
 loop .copy
 jmp .bad
.end:
 cmp di,root_path+3
 jbe .bad
 cmp byte [es:root_path+1],':'
 jne .bad
 cmp byte [es:root_path+2],'\'
 jne .bad
 cmp byte [es:di-1],'\'
 jne .suffix
 dec di
.suffix:
 push cs
 pop ds
 mov si,root_suffix
 mov cx,12
 rep movsb
 sub di,root_path
 mov [es:root_length],di
 jmp .ok
.next:
 dec cx
 jz .bad
 jmp .scan
.bad:
 pop es
 pop ds
 popad
 stc
 ret
.ok:
 pop es
 pop ds
 popad
 clc
 ret
