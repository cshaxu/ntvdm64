
; Close only the explicitly named guest welcome window in this test copy.
dismiss_welcome:
pushad
push word 0
push word 0
push ds
push word welcome_title-data
api 2,50                         ; FindWindow by exact title
or ax,ax
jz .done
push ax
push word 0x10                  ; WM_CLOSE
push word 0
push word 0
push word 0
api 2,110                       ; PostMessage
call settle
.done:
popad
ret
