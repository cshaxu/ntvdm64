from pathlib import Path
import hashlib,json
roots={'current':Path('src/mvdm'),'opennt':Path('O:/repos.external/OpenNT/base/mvdm'),'opennt45':Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')}; names=['inc/WOWMMED.INC','inc/WOWMMCB.INC','wow16/mmsystem/stack.asm','wow16/mmsystem/init.c','wow32/wmmedia.c','wow32/wmmtbl2.h']
out={}
for n in names:
 out[n]={}
 for k,r in roots.items():
  p=r/n;out[n][k]=hashlib.sha256(p.read_bytes()).hexdigest() if p.is_file() else None
m=Path('build/.ninja/msvc-x86-release/wow32/wow32.dll.map')
if m.exists():
 b=m.read_bytes();out['map']={'sha256':hashlib.sha256(b).hexdigest(),'lines':[s for s in b.decode(errors='replace').splitlines() if '_WOW32DriverCallback@28' in s]}
print(json.dumps(out,indent=2))
