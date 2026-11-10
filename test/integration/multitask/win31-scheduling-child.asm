; Authored integration input, not an original guest replacement.
; The external test releases this DOS child only after sampling Win16 progress.
bits 16
org 100h
mov dx,running
mov si,owner
mov cx,3
call write_file
.wait:
mov dx,release
mov ax,3d00h
int 21h
jnc .released
int 28h
jmp .wait
.released:
mov bx,ax
mov ah,3eh
int 21h
mov dx,receipt
mov si,result
mov cx,6
call write_file
mov dx,done
xor cx,cx
call write_file
mov ax,4c00h
int 21h
write_file:
push cx
xor cx,cx
mov ah,3ch
int 21h
pop cx
jc failed
mov bx,ax
mov dx,si
mov ah,40h
int 21h
jc failed
cmp ax,cx
jne failed
mov ah,3eh
int 21h
jc failed
ret
failed:
mov ax,4c01h
int 21h
running: db W31_INSTALL,'\PATCH\SCHED.RUN',0
release: db W31_INSTALL,'\PATCH\SCHED.GO',0
done: db W31_INSTALL,'\PATCH\SCHED.END',0
receipt: db W31_INSTALL,'\PATCH\EXEC.TXT',0
owner: db 'DOS'
result: db 'S14_OK'
