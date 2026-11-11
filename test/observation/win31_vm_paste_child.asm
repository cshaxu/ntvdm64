; Disposable DOS witness of VKD copying and overlapping-paste behavior.
; Result is raw evidence, not an automatic acceptance verdict.
bits 16
org 0x100
start:
 mov ax,0x1684
 mov bx,0x7f83
 xor di,di
 mov es,di
 int 0x2f
 mov ax,es
 or ax,di
 jz failed
 mov [entry],di
 mov [entry+2],es
 mov ax,1
 call far [entry]
 mov [single_status],ax
 mov di,single_keys
 call collect
 mov [single_count],bx
 mov ax,3
 call far [entry]
 mov [single_callbacks],ax
 mov [single_outcomes],cx
 mov ax,2
 call far [entry]
 mov [overlap_status],ax
 mov di,overlap_keys
 call collect
 mov [overlap_count],bx
 mov ax,3
 call far [entry]
 mov [overlap_callbacks],ax
 mov [overlap_outcomes],cx
 mov dx,filename
 xor cx,cx
 mov ah,0x3c
 int 0x21
 jc failed
 mov bx,ax
 mov dx,result
 mov cx,result_end-result
 mov ah,0x40
 int 0x21
 jc failed
 cmp ax,result_end-result
 jne failed
 mov ah,0x3e
 int 0x21
 mov ax,0x4c00
 int 0x21
failed:
 mov ax,0x4c01
 int 0x21
collect:
 ; Diagnostic only: two BIOS seconds, no production timer or polling added.
 push es
 mov ax,0x40
 mov es,ax
 mov bp,[es:0x6c]
 xor bx,bx
.next:
 mov ah,1
 int 0x16
 jz .time
 xor ah,ah
 int 0x16
 mov [di],ax
 add di,2
 inc bx
 cmp bx,8
 jae .done
.time:
 mov ax,[es:0x6c]
 sub ax,bp
 cmp ax,36
 jb .next
.done:
 pop es
 ret
entry: dw 0,0
filename: db 'PASTE.BIN',0
result: db 'PST1'
single_status: dw 0xffff
single_count: dw 0
single_callbacks: dw 0
single_outcomes: dw 0
single_keys: times 8 dw 0
overlap_status: dw 0xffff
overlap_count: dw 0
overlap_callbacks: dw 0
overlap_outcomes: dw 0
overlap_keys: times 8 dw 0
result_end:
