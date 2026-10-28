from pathlib import Path
import hashlib,json
names=['486/dxboot.asm','dxdisk.asm','dxrom.asm','dxrom.inc','gendefs.inc','hostdata.inc','woaswapi.inc','dosx.lnk','makefile']
roots={'current':Path('src/mvdm/dpmi'),'opennt':Path('O:/repos.external/OpenNT/base/mvdm/dpmi'),'opennt45':Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm/dpmi')}
out={}
for n in names:
 out[n]={}
 for label,root in roots.items():
  p=root/n
  out[n][label]={'size':p.stat().st_size,'sha256':hashlib.sha256(p.read_bytes()).hexdigest()} if p.is_file() else None
print(json.dumps(out,indent=2))
