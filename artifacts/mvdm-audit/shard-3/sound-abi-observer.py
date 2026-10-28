from pathlib import Path
import hashlib,json
base=Path('src/mvdm'); roots={'current':base,'opennt':Path('O:/repos.external/OpenNT/base/mvdm'),'opennt45':Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')}; names=['inc/wowsnd.h','wow32/wstbl2.h','wow32/wsman.c','wow16/drivers/sound/sound.asm','wow16/drivers/sound/sound.def','wow32/wowtbl.c']
out={}
for n in names:
 out[n]={}
 for key,root in roots.items():
  p=root/n; out[n][key]=hashlib.sha256(p.read_bytes()).hexdigest() if p.is_file() else None
m=Path('build/.ninja/msvc-x86-release/wow32/wow32.dll.map')
if m.exists():
 b=m.read_bytes();out['observed_map']={'sha256':hashlib.sha256(b).hexdigest(),'lines':[s for s in b.decode(errors='replace').splitlines() if any(k in s for k in ['_WS32DoBeep@4','_InitThunkTableOffsets@0'])]}
print(json.dumps(out,indent=2))
