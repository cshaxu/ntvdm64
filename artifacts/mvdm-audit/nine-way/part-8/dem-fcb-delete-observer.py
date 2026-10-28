import pathlib,hashlib,json
files=['src/mvdm/dos/dem/demfcb.c','O:/repos.external/OpenNT/base/mvdm/dos/dem/demfcb.c','O:/repos.external/OpenNT-4.5/nt/private/mvdm/dos/dem/demfcb.c','src/mvdm/oemuni/file.c','src/mvdm/dos/v86/doskrnl/dos/fcbio.asm','tests/observation/oem_file_contract.asm','tools/audit/Verify-T420S37OemGuest.ps1','docs/etc/evidence/m0-t420-s37-oemuni-capability-review.md'];out=[]
for s in files:
 p=pathlib.Path(s);out.append({'path':s,'sha256':hashlib.sha256(p.read_bytes()).hexdigest().upper() if p.is_file() else None})
pathlib.Path('artifacts/mvdm-audit/nine-way/part-8/dem-fcb-delete-evidence.json').write_text(json.dumps(out,indent=2),encoding='utf8')
