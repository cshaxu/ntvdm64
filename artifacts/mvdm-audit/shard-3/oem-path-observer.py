from pathlib import Path
import hashlib,json
roots={'current':Path('src/mvdm'),'opennt':Path('O:/repos.external/OpenNT/base/mvdm'),'opennt45':Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')}; names=['inc/oemuni.h','oemuni/file.c','oemuni/process.c','dos/dem/demfile.c']
out={}
for n in names:
 out[n]={}
 for k,r in roots.items():
  p=r/n;out[n][k]=hashlib.sha256(p.read_bytes()).hexdigest() if p.is_file() else None
m=Path('build/.ninja/msvc-x86-release/worker/ntvdm.exe.map')
if m.is_file():
 b=m.read_bytes(); keys=['_CreateFileOem@28','_FindFirstFileOem@8','_GetFullPathNameOem@16','_GetShortPathNameOem@12'];out['map']={'sha256':hashlib.sha256(b).hexdigest(),'lines':[s for s in b.decode(errors='replace').splitlines() if any(k in s for k in keys)]}
print(json.dumps(out,indent=2))
