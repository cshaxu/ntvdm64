"""Read-only part 1 bin86 17-file original, ZIP, installation census."""
from pathlib import Path
import csv,hashlib,json,zipfile
own=[x for x in csv.DictReader(Path('artifacts/mvdm-audit/nine-way-ownership.csv').open(encoding='utf-8-sig')) if x['bucket']=='1' and x['kind']=='mvdm' and x['path'].startswith('bin86/')];roots=[Path('src/mvdm'),Path('O:/repos.external/OpenNT/base/mvdm'),Path('O:/repos.external/OpenNT-4.5/nt/private/mvdm'),Path('O:/repos.external/opennt-src-2/nt/private/mvdm')];sha=lambda b:hashlib.sha256(b).hexdigest();rows=[]
with zipfile.ZipFile('assets/winnt.zip') as z:
 for x in own:
  n=x['path'];name=Path(n).name.lower();paths=[r/n for r in roots]; entries=[t for t in z.namelist() if Path(t.replace('\\','/')).name.lower()==name]; i=Path('O:/winnt/system32')/Path(n).name
  rows.append(dict(key=x['key'],identities=[dict(path=str(p),sha256=sha(p.read_bytes()) if p.is_file() else None) for p in paths],zip_entries=[dict(path=t,sha256=sha(z.read(t))) for t in entries],installed_path=str(i),installed_sha256=sha(i.read_bytes()) if i.is_file() else None))
print(json.dumps(dict(files=rows,limitations='Read-only ZIP basename and exact system32 path only; no runtime or source build provenance.'),indent=2))
