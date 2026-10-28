from pathlib import Path
import hashlib,json
roots={'current':Path('src/mvdm'),'opennt':Path('O:/repos.external/OpenNT/base/mvdm'),'opennt45':Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')}; names=['inc/vrinit.h','vdmredir/vrinit.c','vdmredir/vrnetb.c','vdmredir/vrdll.c','vdmredir/vrdisp.c','dos/v86/redir/int5c.asm']
out={}
for n in names:
 out[n]={}
 for k,r in roots.items():
  p=r/n;out[n][k]=hashlib.sha256(p.read_bytes()).hexdigest() if p.is_file() else None
m=Path('artifacts/mvdm-audit/shard-3/redir-private-map.txt')
if m.exists():
 b=m.read_bytes();keys=['_VrHandleAsyncCompletion','_VrQueueCompletionHandler','_VrRaiseInterrupt','_VrUninitialize','_Netbios32Post'];out['map']={'sha256':hashlib.sha256(b).hexdigest(),'lines':[s for s in b.decode(errors='replace').splitlines() if any(k in s for k in keys) and ' f ' in s]}
print(json.dumps(out,indent=2))
