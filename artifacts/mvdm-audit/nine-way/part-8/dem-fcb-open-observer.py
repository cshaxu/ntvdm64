import hashlib,pathlib,json
files=['src/mvdm/dos/dem/demfcb.c','O:/repos.external/OpenNT/base/mvdm/dos/dem/demfcb.c','O:/repos.external/OpenNT-4.5/nt/private/mvdm/dos/dem/demfcb.c','src/mvdm/dos/v86/doskrnl/dos/fcbio.asm','tests/observation/dem_capability.asm','docs/etc/evidence/m0-t420-s33-dem-capability-closure.md'];out=[]
for s in files:
 p=pathlib.Path(s);out.append({'path':s,'sha256':hashlib.sha256(p.read_bytes()).hexdigest().upper()})
pathlib.Path('artifacts/mvdm-audit/nine-way/part-8/dem-fcb-open-evidence.json').write_text(json.dumps(out,indent=2),encoding='utf8')
