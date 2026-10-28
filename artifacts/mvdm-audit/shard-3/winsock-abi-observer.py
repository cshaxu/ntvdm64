from pathlib import Path
import hashlib,json,re
base=Path('src/mvdm'); refs={'current':base,'opennt':Path('O:/repos.external/OpenNT/base/mvdm'),'opennt45':Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')}; names=['inc/wowwsock.h','wow32/wwstbl2.h','wow32/wsraw.c','wow32/wowtbl.c','wow32/winsockp.h']
out={}
for n in names:
 out[n]={}
 for key,r in refs.items():
  p=r/n;out[n][key]=hashlib.sha256(p.read_bytes()).hexdigest() if p.is_file() else None
m=Path('build/.ninja/msvc-x86-release/wow32/wow32.dll.map')
if m.exists():
 b=m.read_bytes(); s=b.decode(errors='replace'); out['observed_map']={'sha256':hashlib.sha256(b).hexdigest(),'lines':[line for line in s.splitlines() if any(k in line for k in ['_WWS32bind@4','_WWS32connect@4','_WWS32socket@4','_WSThunkAddrBuf@16','_InitThunkTableOffsets@0'])]}
print(json.dumps(out,indent=2))
