import collections,csv,hashlib,json,re
from pathlib import Path
repo=Path(__file__).resolve().parents[4];root=Path('O:/repos.external/OpenNT');mini=root/'windows/core/ntgdi/printers/rasprint/mini'
with (repo/'artifacts/mvdm-audit/nine-way/part-9/external-owned-files.csv').open(encoding='utf-8-sig',newline='') as f:
 rows=[r for r in csv.DictReader(f) if r['path'].lower().startswith('windows/core/ntgdi/printers/rasprint/mini/')]
groups=collections.defaultdict(list)
for r in rows:groups[r['path'].split('/')[6]].append(r)
out=[]
for family,owned in sorted(groups.items()):
 d=mini/family;s=d/'sources';contents=s.read_bytes() if s.is_file() else None;rcs=sorted(d.glob('*.rc')) if d.is_dir() else []
 out.append({'family':family,'owned_count':len(owned),'sources_sha256':hashlib.sha256(contents).hexdigest().upper() if contents else None,'sources_text':contents.decode('latin1') if contents else None,'rc_files':[p.name for p in rcs]})
print(json.dumps(out,indent=2))
