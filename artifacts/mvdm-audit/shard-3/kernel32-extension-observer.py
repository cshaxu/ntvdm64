from pathlib import Path
import hashlib,json
roots={'current':Path('src/mvdm'),'opennt':Path('O:/repos.external/OpenNT/base/mvdm'),'opennt45':Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')}; names=['inc/WOWKRN.INC','wow16/kernel31/kthunks.asm','wow32/wktbl2.h','wow32/wkgthunk.c','wow32/wowtbl.c','wow16/mmsystem/init.c']
out={}
for n in names:
 out[n]={}
 for k,r in roots.items():
  p=r/n;out[n][k]=hashlib.sha256(p.read_bytes()).hexdigest() if p.is_file() else None
for n in ['src/wow32-dll/include/wow_user_native_extension.h','src/wow32-dll/source/wow_user_native_extension.c','tools/build/New-T404S5Wow32ProviderNinja.ps1']:
 p=Path(n);out[n]=hashlib.sha256(p.read_bytes()).hexdigest() if p.exists() else None
m=Path('build/.ninja/msvc-x86-release/wow32/wow32.dll.map')
if m.exists():
 b=m.read_bytes();keys=['_WK32FreeLibrary32@4','_WK32GetProcAddress32@4','_WK32LoadLibraryEx32@4'];out['map']={'sha256':hashlib.sha256(b).hexdigest(),'lines':[s for s in b.decode(errors='replace').splitlines() if any(k in s for k in keys)]}
print(json.dumps(out,indent=2))
