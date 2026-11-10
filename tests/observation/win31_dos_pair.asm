; Each DOS program must see its peer before either can return to COMMAND.
bits 16
org 100h
mov ax,1683h                    ; Windows enhanced-mode current VM ID in BX
int 2fh
mov [vm_id],bx
; This setup fixture starts a fresh DOS prompt with no DOS mouse client.
; INT33 exchange returns the prior registration; put it back immediately.
xor cx,cx
xor dx,dx
mov es,dx
mov ax,14h
int 33h
mov [prior_mask],cx
mov ax,0ch
int 33h
mov dx,opened
call write_marker
.wait_peer:
mov ax,1680h                    ; yield the current virtual machine's time slice
int 2fh
mov dx,peer
mov ax,3d00h
int 21h
jc .wait_peer
mov bx,ax
mov ah,3eh
int 21h
jc fail
%if ROLE = 'B'
mov dx,marker
mov ah,9
int 21h
xor ah,ah
int 16h
cmp al,'k'
jne fail
mov dx,first_key
call write_marker
%endif
xor ah,ah
int 16h
cmp al,ROLE+32
jne fail
mov dx,done
call write_marker
mov ax,4c00h
int 21h

write_marker:
xor cx,cx
mov ah,3ch
int 21h
jc fail
mov bx,ax
mov dx,msg
mov cx,msg_end-msg
mov ah,40h
int 21h
jc fail
cmp ax,cx
jne fail
mov ah,3eh
int 21h
jc fail
ret
fail:
mov ax,4c01h
int 21h

opened db W31_INSTALL,'\PATCH\',ROLE,'.RUN',0
peer db W31_INSTALL,'\PATCH\',PEER,'.RUN',0
done db W31_INSTALL,'\PATCH\',ROLE,'.END',0
%if ROLE = 'B'
marker db 'S14_DOS_FULLSCREEN_READY',13,10,'$'
first_key db W31_INSTALL,'\PATCH\B.KEY',0
%endif
msg db ROLE
vm_id dw 0
prior_mask dw 0
msg_end:
