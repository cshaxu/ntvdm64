from pathlib import Path
import hashlib,json
roots={'current':Path('src/mvdm/dos/v86'),'opennt':Path('O:/repos.external/OpenNT/base/mvdm/dos/v86'),'opennt45':Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm/dos/v86')}; names=['inc/dbcs.sw','inc/msghan.inc','inc/msgserv.sa','inc/mshead.asm','inc/mstable.asm','inc/msgserv.asm','doskrnl/dos/mstable.asm','doskrnl/dos/msconst.asm','cmd/command/makefile','doskrnl/dos/makefile']
out={}
for n in names:
 out[n]={}
 for k,r in roots.items():
  p=r/n;out[n][k]={'size':p.stat().st_size,'sha256':hashlib.sha256(p.read_bytes()).hexdigest()} if p.is_file() else None
print(json.dumps(out,indent=2))
