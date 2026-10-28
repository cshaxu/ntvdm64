from pathlib import Path
import hashlib,json
names=['bde.h','bdecmds.c','bdemisc.c','precomp.h','bde.def','sources']
roots={'current':Path('src/mvdm/bde'),'opennt':Path('O:/repos.external/OpenNT/base/mvdm/bde'),'opennt45':Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm/bde')}
out={}
for n in names:
 out[n]={}
 for label,root in roots.items():
  p=root/n
  out[n][label]={'size':p.stat().st_size,'sha256':hashlib.sha256(p.read_bytes()).hexdigest()} if p.is_file() else None
print(json.dumps(out,indent=2))
