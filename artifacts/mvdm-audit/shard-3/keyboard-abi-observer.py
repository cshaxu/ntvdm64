from pathlib import Path
import hashlib,json
roots={'current':Path('src/mvdm'),'opennt':Path('O:/repos.external/OpenNT/base/mvdm'),'opennt45':Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')}; names=['inc/WOWKBD.INC','wow16/drivers/keyboard/keyboard.asm','wow16/drivers/keyboard/kbdlocal.asm','wow16/drivers/keyboard/keyboard.def','wow32/wkbdtbl2.h','wow32/wkbman.c','wow32/wowtbl.c']
out={}
for n in names:
 out[n]={}
 for k,r in roots.items():
  p=r/n;out[n][k]=hashlib.sha256(p.read_bytes()).hexdigest() if p.is_file() else None
m=Path('build/.ninja/msvc-x86-release/wow32/wow32.dll.map')
if m.exists():
 b=m.read_bytes(); keys=['_WKB32ToAscii@4','_WKB32GetKeyNameText@4','_WKB32MapVirtualKey@4','_WKB32VkKeyScan@4'];out['map']={'sha256':hashlib.sha256(b).hexdigest(),'lines':[s for s in b.decode(errors='replace').splitlines() if any(k in s for k in keys)]}
print(json.dumps(out,indent=2))
