from pathlib import Path
import hashlib,json
roots={'current':Path('src/mvdm/dos/v86'),'opennt':Path('O:/repos.external/OpenNT/base/mvdm/dos/v86'),'opennt45':Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm/dos/v86')}; names=['tools/src/buildidx/buildidx.def','tools/src/stripz/stripz.def','tools/src/h2inc/h2inc.mak','tools/src/h2inc/h2inc.doc','tools/src/buildidx/makefile','tools/src/stripz/makefile','messages/makefile','doskrnl/dos/makefile','redir/makefile']
out={}
for n in names:
 out[n]={}
 for k,r in roots.items():
  p=r/n;out[n][k]={'size':p.stat().st_size,'sha256':hashlib.sha256(p.read_bytes()).hexdigest()} if p.is_file() else None
for n in ['tools/build/Build-T225S8PrimaryNtio.ps1','tools/build/Build-T225S8PrimaryNtdos.ps1']:
 p=Path(n);out[n]=hashlib.sha256(p.read_bytes()).hexdigest() if p.exists() else None
print(json.dumps(out,indent=2))
