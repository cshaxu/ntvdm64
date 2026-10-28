import json,pathlib,hashlib
files=['src/mvdm/dos/dem/demfile.c','src/mvdm/dos/dem/demdisp.c','src/mvdm/dos/dem/demerror.c','src/mvdm/dos/v86/doskrnl/dos/file.asm','src/mvdm/oemuni/file.c','src/mvdm/inc/softpc.h','docs/etc/evidence/m0-t404-s5-dem-chmod-ingress-correction-001.md'];out=[]
for s in files:
 p=pathlib.Path(s);out.append({'path':s,'sha256':hashlib.sha256(p.read_bytes()).hexdigest().upper()})
pathlib.Path('artifacts/mvdm-audit/nine-way/part-8/dem-path-guest-host-evidence.json').write_text(json.dumps(out,indent=2),encoding='utf8')
