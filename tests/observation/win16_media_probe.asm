; Independently authored NE acceptance witness; not replacement guest media.
; Original MMSYSTEM public ordinals; independent enumeration/query/error probe.
; Uses100ms unsigned8-bit silence to test asynchronous wave header retirement.
; No input capture, driver installation or settings mutation.
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
resources: dw 4,0
resident: db 8,'MEDPROBE'
dw 0
db 0
modules: dw kernel-imports,user-imports,keyboard-imports
imports:
db 0
kernel: db 6,'KERNEL'
user: db 4,'USER'
keyboard: db 8,'MMSYSTEM'
entries: db 0
entries_end:
%assign calls 0
%macro api 2
%assign calls calls+1
..@call_%[calls]: db 0x9a
dw 0xffff,0
%xdefine module_%[calls] %1
%xdefine ordinal_%[calls] %2
%endmacro

align 16,db 0
code:
api 1,91
test ax,ax
jz fatal
push di
api 2,5
test ax,ax
jz fatal
api 3,401
mov [waveout-data],ax
api 3,501
mov [wavein-data],ax
api 3,201
mov [midiout-data],ax
api 3,301
mov [midiin-data],ax
call snapshot
push ds
push word wavehandle-data
push word 0xffff
push ds
push word format-data
push word 0
push word 0
push word 0
push word 0
push word 0
push word 1
api 3,404
mov [wavequery-data],ax
push ds
push word wavehandle-data
push word 0x7fff
push ds
push word format-data
push word 0
push word 0
push word 0
push word 0
push word 0
push word 1
api 3,404
mov [wavebad-data],ax
call snapshot
push ds
push word midihandle-data
push word 0x7fff
push word 0
push word 0
push word 0
push word 0
push word 0
push word 0
api 3,204
mov [midibad-data],ax
call snapshot
push word 0xfffe
push word 0x0804
push word 0
push word 0
push word 0
push word 0
api 3,701
mov [mcibad-data],ax
push word 0
push word [mcibad-data]
push ds
push word error_text-data
push word 64
api 3,706
mov [mcitext-data],ax
cmp word [wavequery-data],0
jne .wave_end
mov ax,ds
mov [header+2-data],ax
push ds
push word livehandle-data
push word 0xffff
push ds
push word format-data
push word 0
push word 0
push word 0
push word 0
push word 0
push word 0
api 3,404
mov [opened-data],ax
test ax,ax
jnz .wave_end
push word [livehandle-data]
push ds
push word header-data
push word 32
api 3,406
mov [prepared-data],ax
test ax,ax
jnz .wave_reset
push word [livehandle-data]
push ds
push word header-data
push word 32
api 3,408
mov [written-data],ax
test ax,ax
jnz .wave_reset
api 2,13
mov [tick-data],ax
.wave_poll:
api 1,29
test word [header+16-data],1
jnz .wave_done
api 2,13
sub ax,[tick-data]
cmp ax,2000
jb .wave_poll
.wave_done:
mov ax,[header+16-data]
mov [completion_flags-data],ax
.wave_reset:
push word [livehandle-data]
api 3,411
mov [reset_result-data],ax
push word [livehandle-data]
push ds
push word header-data
push word 32
api 3,407
mov [unprepared-data],ax
push word [livehandle-data]
api 3,405
mov [closed-data],ax
.wave_end:
call snapshot
cmp word [midiout-data],0
je .midi_end
push ds
push word midi_live-data
push word 0
push word 0
push word 0
push word 0
push word 0
push word 0
push word 0
api 3,204
mov [midi_opened-data],ax
test ax,ax
jnz .midi_end
push word [midi_live-data]
api 3,205
mov [midi_closed-data],ax
.midi_end:
call snapshot
mov ax,ds
mov [mci_parms+10-data],ax
mov [mci_parms+14-data],ax
push word 0
push word 0x0803
push word 0
push word 0x2200
push ds
push word mci_parms-data
api 3,701
mov [mci_opened-data],ax
mov [mci_opened+2-data],dx
or ax,dx
jnz .mci_end
mov ax,[mci_parms+4-data]
mov [mci_device-data],ax
push ax
push word 0x0804
push word 0
push word 0
push word 0
push word 0
api 3,701
mov [mci_closed-data],ax
mov [mci_closed+2-data],dx
.mci_end:
call snapshot
mov ax,0x4c00
int 0x21
fatal:
mov ax,0x4c01
int 0x21
snapshot:
push ds
push word filename-data
push word 0
api 1,83
mov [file-data],ax
cmp ax,-1
je fatal
push ax
push ds
push word report-data
push word report_end-report
api 1,86
push word [file-data]
api 1,81
ret
code_end:
dw calls
%assign i 1
%rep calls
db 3,1
dw ..@call_%[i]-code+1,module_%[i],ordinal_%[i]
%assign i i+1
%endrep
align 16,db 0
data:
times 32 db 0
report: db 'MED1'
waveout: dw 0xffff
wavein: dw 0xffff
midiout: dw 0xffff
midiin: dw 0xffff
wavequery: dw 0xffff
wavebad: dw 0xffff
midibad: dw 0xffff
mcibad: dw 0xffff
mcitext: dw 0xffff
wavehandle: dw 0
midihandle: dw 0
error_text: times 66 db 0xa5
opened: dw 0xffff
prepared: dw 0xffff
written: dw 0xffff
completion_flags: dw 0xffff
reset_result: dw 0xffff
unprepared: dw 0xffff
closed: dw 0xffff
livehandle: dw 0
midi_opened: dw 0xffff
midi_closed: dw 0xffff
midi_live: dw 0
mci_opened: dd 0xffffffff
mci_closed: dd 0xffffffff
mci_device: dw 0
report_end:
mci_parms: dd 0
dw 0,0
dw mci_type-data,0
dw mci_file-data,0
dd 0
mci_type: db 'waveaudio',0
mci_file: db 'T478.WAV',0
header: dw silence-data,0
dd 800,0,0,0,0,0,0
silence: times 800 db 0x80
tick: dw 0
format: dw 1,1
dd 8000,8000
dw 1,8
file: dw 0
filename: db 'MEDPROBE.BIN',0
data_end:
image_end:
