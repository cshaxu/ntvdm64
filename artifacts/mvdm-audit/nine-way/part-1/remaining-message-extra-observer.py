import csv,io,hashlib,json,subprocess
from pathlib import Path
b=Path('artifacts/mvdm-audit');names=['dos/v86/doskrnl/bios/MSBIO.CL3','dos/v86/doskrnl/bios/MSBIO.CL4','dos/v86/doskrnl/bios/MSBIO.CL5','dos/v86/messages/usa-ms.idx','wow16/ddeml/STRIPPED.DEF'];data=subprocess.check_output(['git','show','b8aaf0870:artifacts/mvdm-audit/combined-initial-status.csv']);c={x['relative_path']:x for x in csv.DictReader(io.StringIO(data.decode('utf-8-sig')))}; roots=[Path('src/mvdm'),Path('O:/repos.external/OpenNT/base/mvdm'),Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm')];sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest() if p.is_file() else None
out=[]
for p in names:
 h=[sha(x/p) for x in roots]; ref=[x for x in h[1:] if x];diff=bool(h[0] and ref and h[0] not in ref);out.append(dict(path=p,status=c.get(p,{}).get('status','missing'),hashes=h,disposition=('remove-from-candidate' if c.get(p,{}).get('status')!='excluded' else 'keep-partial-mirror-diff' if diff else 'scoped-candidate' if h[0] and ref else 'identity-unresolved')))
print(json.dumps(dict(head=subprocess.check_output(['git','rev-parse','b8aaf0870'],text=True).strip(),aggregate_sha256=hashlib.sha256(data).hexdigest(),rows=out,limits='Static identity and current aggregate only, no guest runtime proof.'),indent=2))
