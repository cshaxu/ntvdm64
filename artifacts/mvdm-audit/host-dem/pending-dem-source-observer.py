import hashlib,json
from pathlib import Path
names=('demdasd.c','demdir.c','demerror.c','demioctl.c','demlabel.c','demlock.c','demmsg.c','demmsg.h','dosdef.h')
roots=(Path('src/mvdm/dos/dem'),Path('O:/repos.external/OpenNT/base/mvdm/dos/dem'))
out=[]
for n in names:
    hashes=[hashlib.sha256((root/n).read_bytes()).hexdigest().upper() for root in roots]
    out.append({'path':n,'current_sha256':hashes[0],'opennt_sha256':hashes[1],'equal':hashes[0]==hashes[1]})
Path('artifacts/mvdm-audit/host-dem/pending-dem-source-evidence.json').write_text(json.dumps(out,indent=2)+'\n',encoding='utf-8')
print('all equal:',all(r['equal'] for r in out))
