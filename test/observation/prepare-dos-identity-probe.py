"""Build-only diagnostic composition; never edits production or original guest."""
from pathlib import Path
import re
root = Path(__file__).resolve().parents[2]
out = root / 'build/side-dos-identity'
out.mkdir(exist_ok=False)
cache = root / 'build/.ninja/msvc-x86-release/worker'
ninja = (cache/'build.ninja').read_text()
flags = re.search(r'build obj/adapter-softpc/ntvdm_dos_observer.obj:.*\n  cflags = (.*)', ninja)[1].replace('$:', ':')
inputs = re.search(r'^build ntvdm.exe \| ntvdm.lib: worker_link (.*?) \|', ninja, re.M)[1]
# Explicit replacement object is resolved before its archive member.
link = f'link.exe /nologo /subsystem:console /opt:ref /out:{out}/ntvdm.exe /map:{out}/ntvdm.exe.map /implib:{out}/ntvdm.lib /def:generated/ntvdm-wow32-provider.def {out}/observer.obj {inputs} rpcrt4.lib kernel32.lib user32.lib gdi32.lib advapi32.lib ntdll.lib libcmt.lib libvcruntime.lib libucrt.lib'
(out/'build.cmd').write_text('@echo off\ncall "C:\\Program Files (x86)\\Microsoft Visual Studio\\2022\\BuildTools\\Common7\\Tools\\VsDevCmd.bat" -arch=x86 -host_arch=x64 >nul\nif errorlevel 1 exit /b %errorlevel%\ncd /d '+str(cache)+'\ncl.exe '+flags+' /O2 /DDOS_IDENTITY_LOG=\\"'+out.as_posix()+'/events.log\\" /Fo'+out.as_posix()+'/observer.obj '+root.as_posix()+'/test/observation/dos_identity_observed.c\nif errorlevel 1 exit /b %errorlevel%\n'+link+'\nexit /b %errorlevel%\n')
s=(root/'test/observation/win31_startup_contract.asm').read_text()
prefix=s[:s.index('%ifdef W31_PROMPT_CHECK')]
helpers=s[s.index('.schedule_type:'):s.index('%endif\n.schedule_wait:')]
tail=s[s.index('push ds\npush word ready_name-data'):s.index('%ifdef W31_PROMPT_CHECK',s.index('data:\n'))]
body=''
for role,scan in [('A','0x1e41'),('B','0x3042')]:
    body+=f'''push ds
push word pif_{role}-data
push word 1
api 1,166
cmp ax,32
jb fatal
.focus_{role}:
api 1,29
push word 0
push word 0
push ds
push word title_{role}-data
api 2,50
test ax,ax
jz .focus_{role}
mov [window_{role}-data],ax
push ax
api 2,44
push word [window_{role}-data]
api 2,45
push word [window_{role}-data]
api 2,59
call .schedule_settle
api 2,60
cmp ax,[window_{role}-data]
jne .focus_{role}
mov si,keys_{role}-data
call .schedule_type
mov bx,run_{role}-data
call .wait_file
'''
body+='mov bx,all_done-data\ncall .wait_file\n'
for role in ['A','B']:
    body+=f'''push word [window_{role}-data]
push word 0x10
push word 0
push word 0
push word 0
api 2,110
call .schedule_settle
'''
body+='jmp .done\n'+helpers+'''.wait_file:
push bx
api 1,29
pop bx
push bx
push ds
push bx
push word 0
api 1,85
pop bx
cmp ax,-1
je .wait_file
push ax
api 1,81
ret
.done:
'''
data='schedule_timer: dw 0\nschedule_message: times 18 db 0\n'
for role,scan in [('A','0x1e41'),('B','0x3042')]:
    data+=f"pif_{role}: db W31_INSTALL,'\\ID{role}.PIF',0\ntitle_{role}: db 'Identity {role}',0\nrun_{role}: db W31_INSTALL,'\\PATCH\\{role}.RUN',0\nwindow_{role}: dw 0\nkeys_{role}: dw {scan},0x34be,0x2e43,0x184f,0x324d,0x1c0d,0\n"
data+="all_done: db W31_INSTALL,'\\PATCH\\ALL.DONE',0\ndata_end:\nimage_end:\n"
(out/'controller.asm').write_text(prefix+body+tail+data)
print(out)
