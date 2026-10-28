from pathlib import Path
import hashlib,json
roots={'current':Path('src/mvdm'),'opennt':Path('O:/repos.external/OpenNT/base/mvdm'),'opennt45':Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')}; names=['inc/WOWSHELL.INC','wow32/wshltbl.h','wow32/wshtbl2.h','wow32/wshell.c','wow16/shell/shell.asm','wow32/wowtbl.c']
out={}
for n in names:
 out[n]={}
 for k,r in roots.items():
  p=r/n;out[n][k]=hashlib.sha256(p.read_bytes()).hexdigest() if p.is_file() else None
m=Path('build/.ninja/msvc-x86-release/wow32/wow32.dll.map')
if m.exists():
 b=m.read_bytes(); keys=['_WS32ShellExecute@4','_W32ShellExecuteCallBack@8','_WS32RegOpenKey@4'];out['map']={'sha256':hashlib.sha256(b).hexdigest(),'lines':[s for s in b.decode(errors='replace').splitlines() if any(k in s for k in keys)]}
p=Path('src/wow32-dll/source/wow_shell_callback.c');out['project_callback_sha256']=hashlib.sha256(p.read_bytes()).hexdigest()
print(json.dumps(out,indent=2))
