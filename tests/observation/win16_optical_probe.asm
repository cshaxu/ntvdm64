; Independent DOS client; immutable MSCDEXNT owns VDD lifetime.
bits 16
org 100h
start:
 push cs
 pop ds
 push cs
 pop es
 mov ax,1500h
 xor bx,bx
 int 2fh
 mov [count],bx
 mov [first],cx
 mov ax,150ch
 int 2fh
 mov [version],bx
 mov ax,150bh
 mov cx,OPTICAL_DRIVE
 int 2fh
 mov [iscd],ax
 mov [signature],bx
 mov ax,150dh
 mov bx,letters
 int 2fh
 ; Original TSR IRET preserves caller flags, not host setCF.
 ; No successful data-read claim follows from this carry value.
 mov ax,1508h
 mov cx,OPTICAL_DRIVE
 mov bx,scratch
 xor dx,dx
 xor si,si
 mov di,1
 clc
 int 2fh
 pushf
 pop ax
 and ax,1
 mov [absolute_cf],ax
 mov word [request+14],volume_block
 mov [request+16],ds
 mov word [request+18],5
 call ioctl
 mov [volume_status],ax
 mov eax,[volume_block+1]
 mov [volume],eax
 mov word [request+14],disk_info
 mov word [request+18],7
 call ioctl
 mov [disk_status],ax
 mov byte [request+2],128
 call ioctl
 mov [long_status],ax
 mov byte [request+2],3
 mov word [request+14],volume_block
 mov word [request+18],5
 call ioctl
 mov [repeat_status],ax
 mov eax,[volume_block+1]
 mov [repeat_volume],eax
 mov dx,filename
 xor cx,cx
 mov ah,3ch
 int 21h
 jc fail
 mov bx,ax
 mov dx,report
 mov cx,report_end-report
 mov ah,40h
 int 21h
 jc fail
 cmp ax,report_end-report
 jne fail
 mov ah,3eh
 int 21h
 mov ax,4c00h
 int 21h
fail:
 mov ax,4c01h
 int 21h
ioctl:
 mov word [request+3],0
 mov ax,1510h
 mov cx,OPTICAL_DRIVE
 mov bx,request
 int 2fh
 mov ax,[request+3]
 ret
filename db 'CDPROBE.BIN',0
request db 20,0,3
 dw 0
 times 8 db 0
 db 0
 dw volume_block,0,5
volume_block db 8
 dd 0
report db 'CDP1'
count dw 0
first dw 0
version dw 0
iscd dw 0
signature dw 0
absolute_cf dw 0
volume_status dw 0
disk_status dw 0
long_status dw 0
repeat_status dw 0
volume dd 0
repeat_volume dd 0
disk_info db 10,0,0
 dd 0
letters times 26 db 0ffh
report_end:
scratch times 2048 db 0
